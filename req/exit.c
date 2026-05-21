#include "h_minishell.h"

int	exit_arg(char *v, int *flag)
{
	int	i;

	i = 0;
	while (ft_isspace(v[i]))
		i ++;
	if (v[i] == '+' || v[i] == '-')
		i ++;
	*flag += !v[i];//empty arg //expand should have accounted for quotes, check to see.
	while (ft_isdigit(v[i]))
		i ++;
	while (ft_isspace(v[i]))
		i ++;
	*flag += (v[i] != 0);
	if (*flag)
		return(shell_assert2(1 + 1, v, "numeric argument required\n"));//nl?
	return (ft_atoi(v) % 256);
}

int	exit_builtin(int argc, char **argv, t_env *env, int *fd)
{
    int status;
	int	flag;

	flag = 0;
	ft_putstr_fd("exit\n", 2);//yes it goes to stderr
	if (shell_assert(argc > 2, "exit: too many arguments\n"))//nl is debatable
		return (1);//do not exit on this error
	else if (argc > 1)
		status = exit_arg(argv[1], &flag);
	else
		status = env->last;
	shell_cleanup(env);
	split_cleanup(argv);
	unset(&fd[0]);
	unset(&fd[1]);
//	write(1, "\n", 1);
	exit(status);
	return (0);
}
