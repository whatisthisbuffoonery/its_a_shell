# include "h_minishell.h"

void	cmd_pop(t_cmd **cmd)
{
	t_cmd	*next;

	if (*cmd)
	{
		next = (*cmd)->next;
		cmd_delone(*cmd);
		*cmd = next;
	}
}

//would debug if we used this system
t_cmd	*cmdtrim(t_cmd **list, t_cmd *head, t_cmd *tail)
{
	t_cmd	*prev;
	t_cmd	*next;

	prev = *list;
	while (prev != head && prev->next != head)
		prev = prev->next;
	next = tail->next;
	tail->next = NULL;
	if (prev != head)
		prev->next = next;
	else
		*list = next;
	return (head);
}

//go rename single func later
int	copy_cmd(t_cmd *cmd)
{
	return (cmd != NULL);
}

int	counttype(t_cmd *node, char c)
{
	int	i;

	i = 0;
	while (node)
	{
		i += (node->type == c);
		node = node->next;
	}
	return (i);
}

//does not support rejoining of prior listnorminette
//usage pattern: arg = thisfunc, redir = subcmd('>'->next, isjoined)
t_cmd	*subcmd(t_cmd **index, int (*f)(t_cmd *))
{
	t_cmd	*iter;
	t_cmd	*next;
	t_cmd	*ret;

	iter = *index;//start from node 1
	ret = *index;//remember node 1
	while (f(iter))
		iter = iter->next;//it is just isjoined here
	next = iter;//in ls"a" sumshit, next is "a" due to !"a"->next || "a"->end_space
	if (iter)
	{
		next = iter->next;
		iter->next = NULL;
	}
	*index = next;//index = sumshit
	return (ret);
}
