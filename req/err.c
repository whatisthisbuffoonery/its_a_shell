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
