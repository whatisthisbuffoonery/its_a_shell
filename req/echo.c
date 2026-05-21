#include "h_minishell.h"

int	check_flag(char *v)
{
	if (!v || v[0] != '-')
		return (0);
	return (v[1] == 'n');
}

int	echo(char **argv, int out)//write check
{
	int	flag;
	int	i;

	flag = check_flag(argv[1]);
	i = flag + 1;
	while (argv[i])
	{
		ft_putstr_fd(argv[i], out);
		if (argv[i + 1])
			ft_putchar_fd(' ', out);
		i ++;
	}
	if (!flag)
		ft_putchar_fd('\n', out);
	return (0);
}
