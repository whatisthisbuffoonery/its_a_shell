#include "h_minishell.h"

int	ft_err(int n, char *s)
{
	if (n < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(s, 2);
	}
	return (n);
}
