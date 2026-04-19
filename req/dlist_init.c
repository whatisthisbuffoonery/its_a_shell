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

int	issubshell(t_cst **cst)
{
	if (*cst && (*cst)->brackets && (*cst)->brackets->type == '(')
	{
		cmd_pop(&(*cst)->brackets);
		return (1);
	}
	return (0);
}

t_cmd	*grab_redir(int depth, t_cmd **redir)
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


//null cst stops depth 0 from pulling extra but it does feel fragile
//check for redir_depth == depth - 1
t_dlist	*dlist_redir(t_dlist *dlist, int depth, t_cmd **redir)
{
	t_cst	*cst;

	cst = dlist->cst;
	if (cst && cst->redir && counttype(cst->brackets, ')'))
		*redir = cst->redir;
	if (*redir)
		dlist->redir = grab_redir(depth, redir);
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
	flag = issubshell(cst);
	while (*cst && !dlist->cst && !*complain)
	{
		if (flag)//only check for open bracket
			dlist->down = dlist_init(cst, complain, depth + 1, redir);
		else
			dlist->cst = cst_pop(cst);
	}
	else if (*cst && !*complain)
		dlist->across = dlist_init(cst, complain);
	return (dlist_redir(dlist, depth, redir));
}
