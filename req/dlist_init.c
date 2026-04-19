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
			dlist->down = dlist_init(cst, complain, depth + 1, redir);
		else
			dlist->cst = cst_pop(cst);
	}
	flag = endsubshell(dlist);
	if (*cst && !flag && !*complain)
		dlist->across = dlist_init(cst, complain, depth, redir);
	if (*complain)
		return (dlist);
	else if (dlist->cst && dlist->cst->redir && flag)
		*redir = dlist->cst->redir;
	if (*redir)
		dlist->redir = dlist_redir(depth, redir);
	return (dlist);
}
