#include "h_minishell.h"

int	check_flag(char *v)
{
	if (!v || v[0] != '-')
		return (0);
	return (v[2] == 'n');
}

int	echo(int argc, char **argv)//write check
{
	int	flag;
	int	i;

	flag = check_flag(argv[1]);
	i = flag + 1;
	while (argv[i])
	{
		ft_putstr(argv[i]);
		ft_putchar(' ');
		i ++;
	}
	if (!flag)
		ft_putchar('\n');
	return (0);
}
