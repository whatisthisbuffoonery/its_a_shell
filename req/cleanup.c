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
		clean_cmd(&iter->word_next);
		cmd_delone(iter);
		iter = next;
	}
	*cmd = NULL;
}

void	clean_ast(t_node *node)
{
	if (!node)
		return ;
	clean_cmd(&node->argv);
	clean_cmd(&node->redir_op);
	clean_cmd(&node->redir_target);
	clean_ast(node->redir_next);
	clean_ast(node->left);
	clean_ast(node->right);
	free(node);
}

void	clean_dlist(t_dlist *dlist)
{
	if (!dlist)
		return ;
	clean_cst(&dlist->cst);
	clean_cmd(&dlist->redir);
	clean_dlist(dlist->down);
	clean_dlist(dlist->down_next);
	clean_dlist(dlist->across);
	free(dlist);
}
