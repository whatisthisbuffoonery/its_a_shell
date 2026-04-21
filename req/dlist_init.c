#include "h_minishell.h"

t_cmd	*dlist_redir(int depth, t_cmd **redir)
{
	t_cmd	*iter;
	t_cmd	*head;
	t_cmd	*tail;

	depth --;
	iter = *redir;
	head = NULL;
	tail = NULL;
	while (iter)
	{
		if (iter->depth == depth)
		{
			tail = iter;
			if (!head)
				head = iter;
		}
		iter = iter->next;
	}
	if (!head)
		return (NULL);
	else
		return cmdtrim(redir, head, tail);
}

int	endsubshell(t_dlist *dlist)
{
	while (dlist)
	{
		if (issubshell(&dlist->cst, ')'))
			return (1);
		dlist = dlist->down;
	}
	return (0);
}

int	issubshell(t_cst **cst, char type)
{
	if (*cst && (*cst)->brackets && (*cst)->brackets->type == type)
	{
		cmd_pop(&(*cst)->brackets);
		return (1);
	}
	return (0);
}
void	dlist_append(t_dlist *dst, t_dlist *src)
{
	t_dlist	*iter;
	if (!src)
		return ;
	if (!dst->down)
	{
		dst->down = src;
		return ;
	}
	iter = dst->down_next;//append to parent node, not sibling node
	while (iter && iter->down_next)
		iter = iter->down_next;
	if (!iter)
		dst->down_next = src;
	else
		iter->down_next = src;
}

//(ls && (ls)): _ down > ls across > _ down > ls
//(cmd) && (cmd) will not be joined by across, where (cmd && cmd) will
t_dlist	*dlist_init(t_cst **cst, int *complain, int depth, t_cmd **redir)
{
	t_dlist	*dlist;
	int		flag;

	dlist = ft_calloc(sizeof(t_dlist), 1);
	if (ft_err(-!dlist, "dlist malloc"))
		return (dlist_complain(complain, dlist, NULL));//could merge into cst complain
	while (*cst && !dlist->cst && !*complain)
	{
		if (issubshell(cst, '('))
			dlist_append(dlist, dlist_init(cst, complain, depth + 1, redir));
		else
			dlist->cst = cst_pop(cst);
	}
	flag = endsubshell(dlist);
	if (*cst && !flag && !*complain)
		dlist->across = dlist_init(cst, complain, depth, redir);
	if (*complain)
		return (dlist);
	else if (dlist->cst && dlist->cst->redir && flag)
		*redir = dlist->cst->redir;//redir is local, *redir is caller scope
	if (*redir)
		dlist->redir = dlist_redir(depth, redir);
	if (dlist->cst && dlist->redir == dlist->cst->redir)//and this line pays for it
		dlist->cst->redir = NULL;
	return (dlist);
}
