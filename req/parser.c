#include "h_minishell.h"

static t_node	*node_new(t_node_kind kind)
{
	t_node	*n;

	n = ft_calloc(1, sizeof(t_node));
	if (!n)
	{
		perror("parser: calloc");
		exit(1);
	}
	n->kind = kind;
	return (n);
}

// Is this token a plain word / quoted string / glob star? 
static int	is_word(t_cmd *tok)
{
	char	c;

	if (!tok)
		return (0);
	c = tok->type;
	return (c != '|' && c != '&' && c != '<' && c != '>'
		&& c != '(' && c != ')' && c != '\0');
}

// Is this token a redirect operator? 
static int	is_redir(t_cmd *tok)
{
	if (!tok)
		return (0);
	return (tok->type == '<' || tok->type == '>');
}

// Advance past current token 
static t_cmd	*advance(t_parser *p)
{
	t_cmd	*tok;

	tok = p->cur;
	if (p->cur)
		p->cur = p->cur->next;
	return (tok);
}

static void	parse_err(char *exp, char *got)
{
	write(2, "parse error: expected ", 22);
	write(2, exp, ft_strlen(exp));
	write(2, " got ", 5);
	write(2, got, ft_strlen(got));
	write(2, "\n", 1);
}

// Consume current token only if it matches expected str; return it or NULL 
static t_cmd	*expect_str(t_parser *p, const char *s)
{
	if (!p->cur || ft_strcmp(p->cur->str, (char *)s) != 0)
	{
		if (p->cur)
			parse_err((char *)s, (char *)p->cur->str);
		else
			parse_err((char *)s, "EOF");
		return (NULL);
		p->err = 1;
	}
	return (advance(p));
}

// ── argv builder (for simple commands) ────────────────────────────────── 

static int	count_words(t_cmd *tok)
{
	int n;
	
	n = 0;
	while (tok && (is_word(tok) || is_redir(tok)))
	{
		if (is_word(tok))
			n++;
		tok = tok->next;
	}
	return (n);
}

t_node	 *parse_list(t_parser *p)
{
	t_node		*left;
	t_node		*right;
	t_node		*op_node;
	t_node_kind	kind;

	left = parse_pipeline(p);
	if (!left || p->err)
		return (left);
	while (p->cur
		&& (ft_strcmp(p->cur->str, "&&") == 0 || ft_strcmp(p->cur->str, "||") == 0))
	{
		if (p->cur->str[1] == '&')
			kind = N_AND;
		else
			kind = N_OR;
		advance(p);
		right = parse_pipeline(p);
		if (!right || p->err)
			return (left);			// let caller handle error 
		op_node = node_new(kind);
		op_node->left = left;
		op_node->right = right;
		left = op_node;
	}
	return (left);
}

t_node	 *parse_pipeline(t_parser *p)
{
	t_node	*left;
	t_node	*right;
	t_node	*pipe;

	left = parse_command(p);
	if (!left || p->err)
		return (left);
	while (p->cur
		&& p->cur->type == '|'
		&& ft_strcmp(p->cur->str, "|") == 0)	// not '||' 
	{
		advance(p);
		right = parse_command(p);
		if (!right || p->err)
			return (left);
		pipe = node_new(N_PIPE);
		pipe->left	= left;
		pipe->right = right;
		left = pipe;
	}
	return (left);
}

t_node	 *parse_command(t_parser *p)
{
	if (!p->cur)
	{
		write(2, "parse error: unexpected end of input\n", 37);
		p->err = 1;
		return (NULL);
	}
	if (p->cur->type == '(')
		return (parse_group(p));
	return (parse_simple_cmd(p));
}

t_node	 *parse_group(t_parser *p)
{
	t_node	*g;
	t_node	*body;

	if (!expect_str(p, "("))
		return (NULL);
	body = parse_list(p);
	if (p->err)
		return (body);
	if (!expect_str(p, ")"))
		return (NULL);
	g = node_new(N_GROUP);
	g->left = body;
	g->redir_next = parse_redirects(p);
	return (g);
}

t_node	 *parse_simple_cmd(t_parser *p)
{
	t_node	*cmd;
	t_node	*redir_head;
	t_node	*redir_tail;
	t_node	*r;

	if (!is_word(p->cur) && !is_redir(p->cur))
	{
		if (p->cur)
			parse_err("command", p->cur->str);
		else
			parse_err("command", "EOF");
		p->err = 1;
		return (NULL);
	}
	cmd = node_new(N_CMD);
	redir_head = NULL;
	redir_tail = NULL;
	cmd->argv = ft_calloc(sizeof(char *), (count_words(p->cur) + 1));
	if (!cmd->argv)
		return (NULL);
	while (p->cur && !p->err && (is_word(p->cur) || is_redir(p->cur)))
	{
		if (is_redir(p->cur))
		{
			r = parse_one_redirect(p);
			if (p->err)
				return (cmd);
			if (!redir_tail)
				redir_head = r; // append to redir list 
			else
				redir_tail->redir_next = r;
			redir_tail = r;
		}
		else
		{
			if (!p->cur->end_space && p->cur->next && is_word(p->cur->next))
				cmd->argv[cmd->argc++] = collect_word(p);
			else
			{
				cmd->argv[cmd->argc++] = ft_strdup(p->cur->str); //for easier cleanup
				advance(p);
			}
			cmd->argv[cmd->argc] = NULL;
		}
	}
	cmd->redir_next = redir_head;
	return (cmd);
}

t_node   *parse_one_redirect(t_parser *p)
{
    t_node  *r;

    r = node_new(N_REDIR);
    r->redir_op = p->cur->str;
    advance(p);
    if (!p->cur || !is_word(p->cur))
    {
		write(2, "parse error: expected filename after redirect\n", 46);
		p->err = 1;
        return (r);
    }
    if (!p->cur->end_space && p->cur->next && is_word(p->cur->next))
        r->redir_target = collect_word(p);
    else
    {
        r->redir_target = ft_strdup(p->cur->str);
        advance(p);
    }
    return (r);
}

t_node	 *parse_redirects(t_parser *p)
{
	t_node	*head;
	t_node	*tail;
	t_node	*r;

	head = NULL;
	tail = NULL;
	while (p->cur && is_redir(p->cur))
	{
		r = parse_one_redirect(p);
		if (p->err)
			return (head);
		if (!tail)
			head = r;
		else
			tail->redir_next = r;
		tail = r;
	}
	return (head);
}

char	*collect_word(t_parser *p)
{
	char	*result;
	char	*tmp;
	t_cmd	*prev;

	result = ft_strdup(p->cur->str);
	prev = p->cur;
	advance(p);
	while (p->cur && is_word(p->cur) && !prev->end_space)
	{
		tmp = ft_strjoin(result, p->cur->str);
		free(result);
		result = tmp;
		prev = p->cur;
		advance(p);
	}
	return result;
}

// entry point ──────────────────────────────────────── 
t_node	*parse(t_cmd *tokens)
{
	t_parser	p;
	t_node		*root;

	p.cur = tokens;
	p.err = 0;
	root  = parse_list(&p);
	if (p.err)
	{
		ast_free(root); 
		return (NULL);
	}
	if (p.cur)
	{
		write(2, "parse error: unexpected token '", 31);
		write(2, p.cur->str, ft_strlen(p.cur->str));
		write(2, "'\n", 2);
		ast_free(root);
		return (NULL);
	}
	return (root);
}

// pretty-printer (for testing) ──────────────────────────────────────── 

static void print_indent(int d)
{
	int	i;

	i = 0;
	while (i < d)
	{
		ft_printf("  ");
		i++;
	}
}

static void print_redirs(t_node *r, int depth)
{
	while (r)
	{
		print_indent(depth);
		ft_printf("redirect  %s  %s\n", r->redir_op, r->redir_target);
		r = r->redir_next;
	}
}

void	ast_print(t_node *n, int depth)
{
	int i;

	if (!n)
		return ;
	print_indent(depth);
	if (n->kind == N_CMD)
	{
		ft_printf("Cmd [");
		i = 0;
		while (i < n->argc)
		{
			ft_printf("%s", n->argv[i]);
			if (i + 1 < n->argc)
				ft_printf(", ");
			i++;
		}
		ft_printf("]\n");
		print_redirs(n->redir_next, depth + 1);
	}
	else if (n->kind == N_PIPE)
	{
		ft_printf("Pipe\n");
		ast_print(n->left,	depth + 1);
		ast_print(n->right, depth + 1);
	}
	else if (n->kind == N_AND)
	{
		ft_printf("And\n");
		ast_print(n->left,	depth + 1);
		ast_print(n->right, depth + 1);
	}
	else if (n->kind == N_OR)
	{
		ft_printf("Or\n");
		ast_print(n->left,	depth + 1);
		ast_print(n->right, depth + 1);
	}
	else if (n->kind == N_GROUP)
	{
		ft_printf("Group\n");
		ast_print(n->left, depth + 1);
		print_redirs(n->redir_next, depth + 1);
	}
}
