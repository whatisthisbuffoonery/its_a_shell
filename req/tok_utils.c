# include "h_minishell.h"

void	tok_pop(t_tok **tok)
{
	t_tok	*next;

	if (*tok)
	{
		next = (*tok)->next;
		tok_delone(*tok);
		*tok = next;
	}
}

//would debug if we used this system
t_tok	*toktrim(t_tok **list, t_tok *head, t_tok *tail)
{
	t_tok	*prev;
	t_tok	*next;

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
int	copy_tok(t_tok *tok)
{
	return (tok != NULL);
}

int	counttype(t_tok *node, char c)
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
//usage pattern: arg = thisfunc, redir = subtok('>'->next, isjoined)
t_tok	*subtok(t_tok **index, int (*f)(t_tok *))
{
	t_tok	*iter;
	t_tok	*next;
	t_tok	*ret;

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
