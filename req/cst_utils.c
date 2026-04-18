#include "h_minishell.h"

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
