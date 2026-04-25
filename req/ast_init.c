#include "h_minishell.h"

int	ast_iscond(t_tok *tok)
{
	return (!ft_strcmp(tok->str, "||") || !ft_strcmp(tok->str, "&&"));
}

int	isarg(char c)
{
	return (!isbracket(c) && !iscond(c) && !isredir(c));
}

t_node_kind	find_kind_op(t_tok *tok)
{
	if (!ft_strcmp(tok->str, "&&"))
		return (N_AND);
	else if (!ft_strcmp(tok->str, "||"))
		return (N_OR);
	return (N_ILLEGAL);
}//really isnt necessary but eh

t_node	*node_new(t_node_kind kind, int *complain)
{
	t_node	*ret;

	ret = NULL;
	if (kind != N_ILLEGAL)
		ret = ft_calloc(sizeof(t_node), 1);
	if (!ret)
	{
		*complain = 1;
		if (kind != N_ILLEGAL)
			ft_err(-1, "ast node malloc");
		return (NULL);
	}
	ret->kind = kind;
	return (ret);
}

t_node	*parse(t_tok **tok)
{
	int		stop;
	t_node	*nodes;

	stop = 0;
	nodes = parse_list(tok, &stop);
	if (*tok && !stop)
	{
		ft_putstr_fd("syntax error: unexpected token '", 2);
		ft_putstr_fd((*tok)->str, 2);
		ft_putstr_fd("'\n", 2);
		stop = 1;
	}
	if (stop)
	{
		clean_ast(nodes);
		nodes = NULL;
	}
	return (nodes);
}