#include "h_minishell.h"

int	check_depth(t_cst *cst)
{
	int		depth;
	int		initial;
	t_cmd	*iter;

	depth = cst->depth;
	initial = depth;
	iter = cst->brackets;
	while (iter)
	{
		if (iter->type == '(')
			depth ++;
		else if (iter->type == ')')
			depth --;
		iter = iter->next;
	}
	shell_assert((depth < 0), "negative depth?\n");
//	ft_printf("||depth func||, initial: %d, return: %d\n", initial, depth);
	(void) initial;
	return (depth);
}

int	name_wrapper(t_cst *cst, t_cmd **iter)
{
//	ft_printf("bool: %d\n", counttype(cst->brackets, ')'));
	if (counttype(cst->brackets, ')'))
		return (1);
	if (!cst->cmd)
		cst->cmd = subcmd(iter, isjoined);
	else
		cmd_node_append(&cst->args, subcmd(iter, isjoined));
	return (0);
}

//(ls) > file redirects subshell output to file, I should put redir and depth fields in t_cmd
//or maybe put the bracket as cmd '(' / op ')' (stick ')' together with an actual op)
int	redir_wrapper(t_cst *cst, t_cmd **src)
{
	t_cmd	*redir;
	t_cmd	*arg;
	t_cmd	*iter;
	int		depth;

	redir = *src;
	if (!redir || !redir->next || !isname(redir->next))
		return (1);
	*src = redir->next;
	arg = subcmd(src, isjoined);
	iter = redir;
	depth = check_depth(cst);
	while (iter)
	{
		iter->depth = depth;
		iter = iter->next;
	}
	cmd_node_append(&cst->redir, redir);
	(void) arg;
	return (0);
}

//check if we already have a close bracket and only allow ops and redirs if so
int	meta_wrapper(t_cst *cst, t_cmd **index)
{
	t_cmd	**list;
	char	c;

	c = (*index)->type;
	//if (!iscond(c) && !isbracket(c))
	//	return (1);
	if ((hascommand(cst) && c == '(') || (!hascommand(cst) && c != '('))//don't allow ls() or > redir ()
		return (1);
	else if (!check_depth(cst) && c == ')')
		return (1);
	list = &cst->brackets;
	if (iscond(c))
		list = &cst->op;
	cmd_node_append(list, subcmd(index, single_cmd));
	return (0);
}

//note to allow ((ls) > a) but not ( > a (ls)), which 
//reminder to go back and undo the change to single & type
//call this using the cmd ptr in main, we will just implement moar cleanup funcs for the new types
t_cst	*cst_init(t_cmd **cmd, int *complain, int depth)
{
	t_cst	*cst;

	cst = ft_calloc(sizeof(t_cst), 1);
	if (ft_err(-!cst, "cst malloc"))
		return (cst_complain(complain, cst, NULL));//dont print on NULL
	cst->depth = depth;
	while (*cmd && !cst->op)
	{
		if (isname(*cmd) && name_wrapper(cst, cmd))
			return (cst_complain(complain, cst, (*cmd)->str));
		else if (*cmd && isredir((*cmd)->type) && redir_wrapper(cst, cmd))//seriously??
			return (cst_complain(complain, cst, (*cmd)->str));
		else if (ismeta(*cmd) && meta_wrapper(cst, cmd))
			return (cst_complain(complain, cst, (*cmd)->str));
	}
	if (*cmd)
		cst->next = cst_init(cmd, complain, check_depth(cst));
	else if (!*cmd && (check_depth(cst) || cst->op))
		return (cst_complain(complain, cst, "\0"));
	return (cst);
}
