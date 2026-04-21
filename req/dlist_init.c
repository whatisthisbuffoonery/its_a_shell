#include "h_minishell.h"


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

int	issubshell(t_cst **cst, char type)
{
	if (*cst && (*cst)->brackets && (*cst)->brackets->type == type)
	{
		cmd_pop(&(*cst)->brackets);
		return (1);
	}
	return (0);
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


//null cst stops depth 0 from pulling extra but it does feel fragile
//check for redir_depth == depth - 1
		/*
t_dlist	*dlist_redir(t_dlist *dlist, int depth, t_cmd **redir)
{
	t_cst	*cst;

	cst = dlist->cst;
}
*/

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

t_cst	*cst_pop(t_cst **cst)
{
	t_cst	*ret;

	if (!*cst)
		return (NULL);
	ret = *cst;
	*cst = ret->next;
	ret->next = NULL;
	return (ret);
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
	else if (dlist->cst && dlist->cst->redir && flag)//make a wrapper
		*redir = dlist->cst->redir;
	if (*redir)
		dlist->redir = dlist_redir(depth, redir);
	if (dlist->cst && dlist->redir == dlist->cst->redir)
		dlist->cst->redir = NULL;
	return (dlist);
}
