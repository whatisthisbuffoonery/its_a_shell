#include "h_minishell.h"

void	clean_shnode(t_shnode **shnode)
{
	t_shnode	*iter;
	t_shnode	*next;

	iter = *shnode;
	while (iter)
	{
		next = iter->next;
		free(iter->name);
		free(iter->str);
		free(iter);
		iter = next;
	}
	*shnode = NULL;
}

void	clean_shnode_dup(t_shnode **shnode)
{
	t_shnode	*iter;
	t_shnode	*next;

	iter = *shnode;
	while (iter)
	{
		next = iter->next;
		free(iter);
		iter = next;
	}
	*shnode = NULL;
}

void	cmd_delone(t_cmd *cmd)
{
	free(cmd->str);
	clean_shnode_dup(&cmd->env);
	free(cmd);
}

void	clean_cmd(t_cmd **cmd)
{
	t_cmd	*iter;
	t_cmd	*next;

	iter = *cmd;
	while(iter)
	{
		next = iter->next;
		cmd_delone(iter);
		iter = next;
	}
	*cmd = NULL;
}
