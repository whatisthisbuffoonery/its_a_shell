#include "h_minishell.h"

int	shell_assert(int cond, char *s)
{
	if (cond)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(s, 2);
	}
	return (cond);
}

int	ft_err(int n, char *s)
{
	if (n < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(s);
	}
	return (n);
}

//token: `a' //yes its backtick `, symbol a, single quote '
t_cst	*cst_complain(int *complain, t_cst *cst, char *s)
{
	if (s)
		ft_putstr_fd("minishell: ", 2);
	if (s && s[0])
	{
		ft_putstr_fd("unexpected token near `", 2);
		ft_putstr_fd(s, 2);
		ft_putstr_fd("\'\n", 2);
	}
	else if (s && !s[0])
	{
		ft_putstr_fd("syntax error: unexpected end of file\n", 2);
		//should the shell exit?
	}
	*complain = 1;
	return (cst);
}
t_dlist	*dlist_complain(int *complain, t_dlist *dlist, char *s)
{
	if (s)
		ft_putstr_fd("minishell: ", 2);
	if (s && s[0])
	{
		ft_putstr_fd("unexpected token near `", 2);
		ft_putstr_fd(s, 2);
		ft_putstr_fd("\'\n", 2);
	}
	else if (s && !s[0])
	{
		ft_putstr_fd("syntax error: unexpected end of file", 2);
		//should the shell exit?
	}
	*complain = 1;
	return (dlist);
}
