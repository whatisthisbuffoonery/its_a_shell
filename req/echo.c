#include "h_minishell.h"

int	check_flag(char *v)
{
	if (!v || v[0] != '-')
		return (0);
	return (v[2] == 'n');
}

int	echo(int argc, char **argv)
{
	int	flag;
	int	i;

	flag = check_flag(v[1]);
	i = flag + 1;
	while (argv[i])
	{
		ft_putstr(v[i]);
		ft_putchar(' ');
		i ++;
	}
	if (!flag)
		ft_putchar('\n');
	return (0);
}
