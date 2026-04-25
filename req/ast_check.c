#include "h_minishell.h"

int	single_cmd(t_cmd *iter)
{
	(void) iter;
	return (0);
}

int	isname(t_cmd *node)
{
	return (node && (iscontent(node->type) || ft_isquote(node->type)
			|| node->type == '*'));
}

int	isjoined(t_cmd *node)
{
	return (isname(node) && isname(node->next) && !node->end_space);
}

int	ismeta(t_cmd *cmd)
{
	return (cmd && (iscond(cmd->type) || isbracket(cmd->type)));
}
