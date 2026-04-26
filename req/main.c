#include "h_minishell.h"

volatile sig_atomic_t	muh_number;


int	shell_exit(t_env *env)
{
	ft_putstr("exiting now\n");
	rl_clear_history();
	clean_shnode_dup(&env->env);
	clean_shnode(&env->export);
	clean_ast(env->ast);//should not really do anything
	return (env->last);
}

void	print_more_tok(t_tok *tok)
{
	print_tok(&tok);
	if (tok)
		print_env(tok->env);
}

void	print_linear_tok(t_tok *tok, char *s)
{
	if (s)
	{
		ft_putstr(s);
		ft_putstr(": ");
	}
	while (tok)
	{
	//	ft_printf("[%s] ", tok->str);
		print_word(tok);
		ft_putchar (' ');
		tok = tok->next;
	}
	if (s)
		ft_putstr("\n");
}

int	isempty(char *buf)
{
	int	i;

	i = 0;
	while (buf[i] && ft_isspace(buf[i]))
		i ++;
	return (!buf[i]);
}

void	init(t_env *env, char **e, t_tok **tok)
{
	signal_init();
	*tok = NULL;
	env_init(env, e);
	env->ast = NULL;
	env->last = 0;
	env->do_not_subshell = 0;
	env->duped_fd[0] = 0;
	env->duped_fd[1] = 0;
}

//need to update last in execution
int	execute_buffer(t_env *env, t_tok **tok)
{
	make_word(*tok);
	env->ast = parse(tok);
	if (env->ast)
		print_ast(env->ast, 0);
	clean_ast(env->ast);
	env->ast = NULL;
	return (env->last);
}

int loop(char **e)
{
	char	*buf;
	t_env	env;
	t_tok	*tok;

	init(&env, e, &tok);
	while (1)
	{
		buf = readline("I am a shell% ");
		if (!buf)
			return (shell_exit(&env));
		else if (!isempty(buf))
		{
			env.last = tok_init(buf, &tok);
			add_history(buf);
			free(buf);
			buf = NULL;
			if (!env.last)
				execute_buffer(&env, &tok);
			print_linear_tok(tok, "thing");
		}
		free(buf);
		clean_tok(&tok);
		muh_number = 0;
	}
	return (env.last);
}

int	main(int c, char **v, char **e)
{
	(void) c;
	(void) v;
	return (loop(e));
}
