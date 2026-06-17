#include "h_minishell.h"

t_node_kind	find_kind_op(t_tok *tok)
{
	if (!ft_strcmp(tok->str, "&&"))
		return (N_AND);
	else if (!ft_strcmp(tok->str, "||"))
		return (N_OR);
	return (N_ILLEGAL);
}

int	group_kind(char c)
{
	return (c == '(');
}
