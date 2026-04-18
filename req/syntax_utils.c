#include "h_minishell.h"

int	isjoined(t_cmd *node)
{
	return (node && !node->end_space && (iscontent(node->type) || node->type == '*'));
}

int	isname(t_cmd *node)
{
	return (node && (iscontent(node->type) || node->type == '*'));
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

	iter = *index;
	ret = *index;
	while (f(iter))
		iter = iter->next;
	next = iter;
	if (iter)
	{
		next = iter->next;
		iter->next = NULL;
	}
	*index = next;
	return (ret);
}

void	name_wrapper(t_cst *cst, t_cmd **iter)
{
	if (!cst->cmd)
		cst->cmd = subcmd(iter, isjoined);
	else
		cmd_node_append(&cst->args, subcmd(iter, isjoined));
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
		ft_putstr_fd("minishell: unexpected token near `");
		ft_putstr_fd(s);
		ft_putstr_fd("\'\n");
	}
	*complain = 1;
	return (cst);
}


int	check_depth(t_cst *cst)
{
	int		depth;
	t_cmd	*iter;

	depth = cst->depth;
	if (!depth->brackets)
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

//check if we already have a close bracket and only allow ops and redirs if so
int	meta_wrapper(t_cst *cst, t_cmd **index)
{
	t_cmd	**list;
	t_cmd	*src;
	char	c;

	src = *index;
	c = src->type;
	if (!iscond(c) && !isbracket(c))
		return (1);
	else if (cst->cmd && c == '(')//don't allow ls()
		return (1);
	else if (!cst->cmd && c != '(')
		return (1);
	else if (!check_depth(cst) && c == ')')
		return (1);
//	if (counttype(cst->brackets, c))
//		return (1)
	list = &cst->brackets;
	if (iscond(c))
		list = &cst->op;
	cmd_node_append(&cst->brack)//hmm
}

//note to allow ((ls) > a) but not ( > a (ls))
//reminder to go back and undo the change to single & type
//call this using the cmd ptr in main, we will just implement moar cleanup funcs for the new types
t_cst	*cst_init(t_cmd **cmd, int *complain, int depth)
{
	t_cst	*cst;
	t_cmd	*iter;

	iter = *cmd;
	cst = ft_calloc(sizeof(t_cst), 1);
	if (ft_err(-!cst, "cst malloc"))
		return (cst_complain(complain, cst, NULL));//dont print on NULL
	cst->depth = depth;
	while (iter && !cst->op)
	{
		if (isname(iter))//get this to check for close brackets
			name_wrapper(cst, &iter);
		else if (isredir(iter->type) && redir_wrapper(cst, &iter))
			return (cst_complain(complain, iter->str));//syntax err
	//	else if (iscond(iter->type))
	//		cst->op = subcmd(&iter, single);
		else if (meta_wrapper(cst, &iter))
			return (cst_complain(complain, cst, iter->str));
	}
	if (iter)
		cst->next = cst_init(&iter, complain, check_depth(cst));
	return (cst);//have a checker for check_depth(cst) && !iter
}
