#include "h_minishell.h"

static void print_indent(int d)
{
	int	i;

	i = 0;
	while (i < d)
	{
		ft_printf("    ");
		i++;
	}
}

static void print_redirs(t_node *r, int depth)
{
	if (!r)
		return ;
	print_indent(depth);
	ft_putstr("redirect{");
	while (r)
	{
		ft_putstr(r->redir_op->str);
		print_word(r->redir_target);
		r = r->redir_next;
	}
	ft_putstr("}\n");
}

void	print_cmd_node(t_node *n)
{
	print_linear_tok(n->argv, "argv");
}

void	print_ast(t_node *n, int depth)
{
	if (!n)
		return ;
	print_indent(depth++);
	if (n->kind == N_CMD)
	{
		print_cmd_node(n);
		print_redirs(n->redir_next, depth);
	}
	else if (n->kind == N_GROUP)
	{
		ft_putstr("Group\n");
		print_ast(n->left, depth);
		print_redirs(n->redir_next, depth);
	}
	else
	{
		if (n->kind == N_PIPE)
			ft_putstr("Pipe\n");
		else if (n->kind == N_AND)
			ft_putstr("And\n");
		else if (n->kind == N_OR)
			ft_putstr("Or\n");
		print_ast(n->left, depth);
		print_ast(n->right, depth);
	}
}
