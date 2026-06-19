#include "h_minishell.h"

int	cd_exit(int status, char *argv, char *newpwd)
{
	if (!status && !ft_strcmp(argv, "-") && newpwd)
	{
		ft_putstr(newpwd);
		ft_putchar('\n');
	}
	return (status);
}
