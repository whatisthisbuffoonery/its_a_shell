#include "h_minishell.h"

int	isname(t_cmd *node)
{
	return (node && (iscontent(node->type) || ft_isquote(node->type)
			|| node->type == '*'));
}

int	isjoined(t_cmd *node)
{
	return (isname(node) && isname(node->next) && !node->end_space);
}

//I wouldn't plug this in
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

int	name_wrapper(t_cst *cst, t_cmd **iter)
{
	ft_printf("bool: %d\n", counttype(cst->brackets, ')'));
	if (counttype(cst->brackets, ')'))
		return (1);
	if (!cst->cmd)
		cst->cmd = subcmd(iter, isjoined);
	else
		cmd_node_append(&cst->args, subcmd(iter, isjoined));
	return (0);
}

int	single(t_cmd *iter)
{
	(void) iter;
	return (0);
}

//token: `a' //yes its backtick `, symbol a, single quote '
t_cst	*cst_complain(int *complain, t_cst *cst, char *s)
{
	if (s)
	{
		ft_putstr_fd("minishell: unexpected token near `", 2);
		ft_putstr_fd(s, 2);
		ft_putstr_fd("\'\n", 2);
	}
	*complain = 1;
	return (cst);
}

int	check_depth(t_cst *cst)
{
	int		depth;
	t_cmd	*iter;

	depth = cst->depth;
	if (!cst->brackets)
		return (depth);
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
	return (depth);
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
	iter = arg;
	depth = check_depth(cst);
	while (iter)
	{
		iter->depth = depth;
		iter = iter->next;
	}
	cmd_node_append(&cst->redir, redir);
	return (0);
}

int	hascommand(t_cst *cst)
{
	return (cst->cmd || cst->redir);
}

int	ismeta(t_cmd *cmd)
{
	return (cmd && (iscond(cmd->type) || isbracket(cmd->type)));
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
	cmd_node_append(list, subcmd(index, single));
	return (0);
}

//note to allow ((ls) > a) but not ( > a (ls)), which 
//reminder to go back and undo the change to single & type
//call this using the cmd ptr in main, we will just implement moar cleanup funcs for the new types
t_cst	*cst_init(t_cmd **cmd, int *complain, int depth)
{
	t_cst	*cst;
//	t_cmd	*iter;

//	iter = *cmd;
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
	return (cst);//have a checker for check_depth(cst) && !iter
}
