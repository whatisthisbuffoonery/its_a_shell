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

void	print_more_cmd(t_cmd *cmd)
{
	print_cmd(&cmd);
	if (cmd)
		print_env(cmd->env);
}

void	print_linear_cmd(t_cmd *cmd, char *s)
{
	if (s)
	{
		ft_putstr(s);
		ft_putstr(": ");
	}
	while (cmd)
	{
		ft_printf("[%s] ", cmd->str);
		cmd = cmd->next;
	}
	if (s)
		ft_putstr("\n");
}

void	print_linear_cst(t_cst *cst)
{
	while (cst)
	{
		print_linear_cmd(cst->cmd, "cmd");
		print_linear_cmd(cst->args, "args");
		print_linear_cmd(cst->redir, "redir");
		print_linear_cmd(cst->op, "op");
		ft_putchar('\n');
		cst = cst->next;
	}
}

//does not handle brackets, that will be handled in shlist print instead
void	print_cst(t_cst *cst, int check)
{
	while (cst)
	{
		print_more_cmd(cst->cmd);
		print_more_cmd(cst->args);
		print_more_cmd(cst->redir);
		print_more_cmd(cst->op);

		cst = cst->next;
	}
	if (!check)
		ft_printf("no syntax errors found prior to checking empty brackets / missing command in tail\n");
}

void	clean_cst(t_cst **cst)
{
	t_cst	*iter;
	t_cst	*next;

	iter = *cst;
	while (iter)
	{
		clean_cmd(&iter->cmd);
		clean_cmd(&iter->args);
		clean_cmd(&iter->op);
		clean_cmd(&iter->redir);
		clean_cmd(&iter->brackets);
		next = iter->next;
		free(iter);
		iter = next;
	}
	*cst = NULL;
}

int main(int c, char **v, char **e)
{
	int		last;//put inside env
	char	*buf;//put inside env...?
	t_env	env;
	t_cmd	*cmd;
	t_cmd	*tmp;
	t_cst	*cst;
	t_cmd	*redir;
	t_dlist	*dlist;

	signal_init();
	cmd = NULL;
	tmp = NULL;
	cst = NULL;
	dlist = NULL;
	redir = NULL;
	env_init(&env, e);
	//env_print(&env);
	while (1)
	{
		last = 0;
		buf = readline("I am a shell% ");
		if (/*!muh_number &&*/ !buf)
			return (shell_exit(&env, last));
		if (buf[0])
		{
			cmd_init(buf, &cmd);//reminder to check for parsing failure
			make_word(cmd);
			//shell_print(&cmd, buf, &env);
			print_cmd(&cmd);
		//	print_word(cmd);
			//ft_printf("is it joined: %d, what end_space: %d\n", isjoined(cmd), cmd->end_space);
			add_history(buf);
//			cst = cst_init(&cmd, &last, 0, NULL);
			//print_cst(cst, last);
/*			if (!last)
			{
				dlist = dlist_init(&cst, &last, 0, &redir);
				print_dlist(dlist, 0);
				print_dlist_digestible(dlist);
				ft_putchar('\n');
			}
			clean_cst(&cst);
*/		}
		free(buf);
		clean_cmd(&cmd);
		clean_cst(&cst);
		clean_dlist(dlist);
		dlist = NULL;
		muh_number = 0;
	}
	(void) c;
	(void) v;
	(void) cst;
	(void) tmp;
	(void) redir;
}
