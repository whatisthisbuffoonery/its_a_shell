#include "h_minishell.h"

int	single(t_cmd *iter)
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

int	hascommand(t_cst *cst)
{
	return (cst->cmd || cst->redir);
}

int	ismeta(t_cmd *cmd)
{
	return (cmd && (iscond(cmd->type) || isbracket(cmd->type)));
}
