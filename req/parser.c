#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "h_minishell.h"

static t_node   *node_new(t_node_kind kind)
{
    t_node  *n;

    n = calloc(1, sizeof(t_node));
    if (!n)
    {
        perror("parser: calloc");
        exit(1);
    }
    n->kind = kind;
    return (n);
}

// Is this token a plain word / quoted string / glob star? 
static int  is_word(t_cmd *tok)
{
    char    c;

    if (!tok)
        return (0);
    c = tok->type;
    return (c != '|' && c != '&' && c != '<' && c != '>'
        && c != '(' && c != ')' && c != '\0');
}

// Is this token a redirect operator? 
static int  is_redir(t_cmd *tok)
{
    if (!tok)
        return (0);
    return (tok->type == '<' || tok->type == '>');
}

// Advance past current token 
static t_cmd    *advance(t_parser *p)
{
    t_cmd   *tok;

    tok = p->cur;
    if (p->cur)
        p->cur = p->cur->next;
    return (tok);
}

// Consume current token only if it matches expected str; return it or NULL 
static t_cmd    *expect_str(t_parser *p, const char *s)
{
    if (!p->cur || strcmp(p->cur->str, s) != 0)
    {
        fprintf(stderr, "parse error: expected '%s' got '%s'\n",
            s, p->cur ? p->cur->str : "EOF");
        p->err = 1;
        return (NULL);
    }
    return (advance(p));
}

// ── argv builder (for simple commands) ────────────────────────────────── 

static void argv_push(t_node *n, char *s)
{
    char    **tmp;

    tmp = realloc(n->argv, sizeof(char *) * (n->argc + 2));
    if (!tmp)
    {
        perror("parser: realloc argv");
        exit(1);
    }
    n->argv = tmp;
    n->argv[n->argc++] = s;
    n->argv[n->argc]   = NULL;
}
t_node   *parse_list(t_parser *p)
{
    t_node      *left;
    t_node      *right;
    t_node      *op_node;
    t_node_kind  kind;

    left = parse_pipeline(p);
    if (!left || p->err)
        return (left);

    while (p->cur
        && (strcmp(p->cur->str, "&&") == 0 || strcmp(p->cur->str, "||") == 0))
    {
        kind = (p->cur->str[1] == '&') ? N_AND : N_OR;
        advance(p);
        right = parse_pipeline(p);
        if (!right || p->err)
            return (left);          // let caller handle error 
        op_node = node_new(kind);
        op_node->left  = left;
        op_node->right = right;
        left = op_node;
    }
    return (left);
}

t_node   *parse_pipeline(t_parser *p)
{
    t_node  *left;
    t_node  *right;
    t_node  *pipe;

    left = parse_command(p);
    if (!left || p->err)
        return (left);

    while (p->cur
        && p->cur->type == '|'
        && strcmp(p->cur->str, "|") == 0)   // not '||' 
    {
        advance(p);
        right = parse_command(p);
        if (!right || p->err)
            return (left);
        pipe = node_new(N_PIPE);
        pipe->left  = left;
        pipe->right = right;
        left = pipe;
    }
    return (left);
}

t_node   *parse_command(t_parser *p)
{
    if (!p->cur)
    {
        fprintf(stderr, "parse error: unexpected end of input\n");
        p->err = 1;
        return (NULL);
    }
    if (p->cur->type == '(')
        return (parse_group(p));
    return (parse_simple_cmd(p));
}

t_node   *parse_group(t_parser *p)
{
    t_node  *g;
    t_node  *body;

    if (!expect_str(p, "("))
        return (NULL);
    body = parse_list(p);
    if (p->err)
        return (body);
    if (!expect_str(p, ")"))
        return (NULL);
    g = node_new(N_GROUP);
    g->left       = body;
    g->redir_next = (t_node *)((void *)parse_redirects(p)); // see note 
    return (g);
}

t_node   *parse_simple_cmd(t_parser *p)
{
    t_node  *cmd;
    t_node  *redir_head;
    t_node  *redir_tail;
    t_node  *r;

    if (!is_word(p->cur) && !is_redir(p->cur))
    {
        fprintf(stderr, "parse error: expected command, got '%s'\n",
            p->cur ? p->cur->str : "EOF");
        p->err = 1;
        return (NULL);
    }
    cmd        = node_new(N_CMD);
    redir_head = NULL;
    redir_tail = NULL;

    while (p->cur && !p->err && (is_word(p->cur) || is_redir(p->cur)))
    {
        if (is_redir(p->cur))
        {
            r = node_new(N_REDIR);
            r->redir_op     = p->cur->str;
            advance(p);
            if (!p->cur || !is_word(p->cur))
            {
                fprintf(stderr, "parse error: expected filename after redirect\n");
                p->err = 1;
                return (cmd);
            }
            r->redir_target = p->cur->str;
            advance(p);
            // append to redir list 
            if (!redir_tail)
                redir_head = r;
            else
                redir_tail->redir_next = r;
            redir_tail = r;
        }
        else
        {
            argv_push(cmd, p->cur->str);
            advance(p);
        }
    }
    cmd->redir_next = redir_head;
    return (cmd);
}

t_node   *parse_redirects(t_parser *p)
{
    t_node  *head;
    t_node  *tail;
    t_node  *r;

    head = NULL;
    tail = NULL;
    while (p->cur && is_redir(p->cur))
    {
        r = node_new(N_REDIR);
        r->redir_op = p->cur->str;
        advance(p);
        if (!p->cur || !is_word(p->cur))
        {
            fprintf(stderr, "parse error: expected filename after redirect\n");
            p->err = 1;
            return (head);
        }
        r->redir_target = p->cur->str;
        advance(p);
        if (!tail)
            head = r;
        else
            tail->redir_next = r;
        tail = r;
    }
    return (head);
}

// entry point ──────────────────────────────────────── 
t_node  *parse(t_cmd *tokens)
{
    t_parser    p;
    t_node      *root;

    p.cur = tokens;
    p.err = 0;
    root  = parse_list(&p);
    if (p.err)
    {
        // TODO: ast_free(root); 
        return (NULL);
    }
    if (p.cur)
    {
        fprintf(stderr, "parse error: unexpected token '%s'\n", p.cur->str);
        return (NULL);
    }
    return (root);
}

// pretty-printer (for testing) ──────────────────────────────────────── 

static void print_indent(int d)
{
    for (int i = 0; i < d; i++)
        printf("  ");
}

static void print_redirs(t_node *r, int depth)
{
    while (r)
    {
        print_indent(depth);
        printf("redirect  %s  %s\n", r->redir_op, r->redir_target);
        r = r->redir_next;
    }
}

void    ast_print(t_node *n, int depth)
{
    int i;

    if (!n)
        return ;
    print_indent(depth);
    if (n->kind == N_CMD)
    {
        printf("Cmd [");
        for (i = 0; i < n->argc; i++)
            printf("%s%s", n->argv[i], (i + 1 < n->argc) ? ", " : "");
        printf("]\n");
        print_redirs(n->redir_next, depth + 1);
    }
    else if (n->kind == N_PIPE)
    {
        printf("Pipe\n");
        ast_print(n->left,  depth + 1);
        ast_print(n->right, depth + 1);
    }
    else if (n->kind == N_AND)
    {
        printf("And\n");
        ast_print(n->left,  depth + 1);
        ast_print(n->right, depth + 1);
    }
    else if (n->kind == N_OR)
    {
        printf("Or\n");
        ast_print(n->left,  depth + 1);
        ast_print(n->right, depth + 1);
    }
    else if (n->kind == N_GROUP)
    {
        printf("Group\n");
        ast_print(n->left, depth + 1);
        print_redirs(n->redir_next, depth + 1);
    }
}
