#include "h_minishell.h"

t_node_kind	find_kind_op(t_tok *tok)
{
	(void) tok;
	return (N_CMD);
}

int	group_kind(char c)
{
	(void) c;
	return (0);
}
