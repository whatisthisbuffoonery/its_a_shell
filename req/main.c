#include "h_minishell.h"

volatile sig_atomic_t	muh_number;


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
	//	ft_printf("[%s] ", cmd->str);
		print_word(cmd);
		ft_putchar (' ');
		cmd = cmd->next;
	}
	if (s)
		ft_putstr("\n");
}

int main(int c, char **v, char **e)
{
	int		last;//put inside env
	char	*buf;//put inside env...?
	t_env	env;
	t_cmd	*cmd;
	t_node	*ast;

	signal_init();
	cmd = NULL;
	env_init(&env, e);
	//env_print(&env);
	while (1)
	{
		ast = NULL;
		last = 0;
		buf = readline("I am a shell% ");
		if (/*!muh_number &&*/ !buf)
			return (shell_exit(&env, last));
		if (buf[0])
		{
			cmd_init(buf, &cmd);//reminder to check for parsing failure
			make_word(cmd);
			ast = parse(&cmd);
			if (ast)
				print_ast(ast, 0);
			clean_ast(ast);
			print_linear_cmd(cmd, "thing");
			add_history(buf);
		}
		free(buf);
		clean_cmd(&cmd);
		muh_number = 0;
	}
	(void) c;
	(void) v;
}
