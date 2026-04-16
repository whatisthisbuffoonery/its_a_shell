#include "h_minishell.h"

volatile sig_atomic_t	muh_number;

/*shnode utilities------------------------------------------------------------*/

void	do_thing(char *buf)
{
	char *arr[3];
	pid_t pid;

	(void) buf;
	ft_putstr("<thing> ");
	arr[0] = "/usr/bin/sleep";
	arr[1] = "3";
	arr[2] = NULL;
	pid = fork();
	if (!pid)
	{
		execve(arr[0], arr, NULL);
		perror("huh");
		exit(67);
	}
	/*
	while (w)
	{
		pid = wait(NULL);
		errno = 0;
		flag -= (pid < 0);
		if (flag)
		{
			ft_putnbr(pid);		//sig int causes this to immediately return -1
			ft_putchar('\n');
		}
		if (errno || pid < 0)
		{
			perror("error");
		}
	}
	*/
	while (waitpid(pid, NULL, 0) < 0);
	ft_putstr("done\n");
}

int	shell_exit(t_env *env, int last)
{
	ft_putstr("exiting now\n");
	rl_clear_history();
	clean_shnode_dup(&env->env);
	clean_shnode(&env->export);
	return (last);
}

int main(int c, char **v, char **e)
{
	int		last;
	char	*buf;
	t_env	env;
	t_cmd	*cmd;

	(void) c;
	(void) v;
//	signal_init();
	cmd = NULL;
	env_init(&env, e);
//	env_print(&env);
	last = 0;
	while (1)
	{
		buf = readline("I am a shell% ");
		shell_print(&cmd, buf, &env);
		if (/*!muh_number &&*/ !buf)
			return (shell_exit(&env, last));
		clean_cmd(&cmd);
		muh_number = 0;
	}
}
