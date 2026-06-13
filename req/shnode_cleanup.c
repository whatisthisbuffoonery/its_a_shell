#include "h_minishell.h"

void	*clean_one_shnode(t_shnode *node)
{
	free(node->name);
	free(node->str);
	free(node);
	return (NULL);
}

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