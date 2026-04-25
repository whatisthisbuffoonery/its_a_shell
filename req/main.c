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

int main(int c, char **v, char **e)
{
	int		last;//put inside env
	char	*buf;//put inside env...?
	t_env	env;
	t_tok	*tok;
	t_node	*ast;

	signal_init();
	tok = NULL;
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
			tok_init(buf, &tok);//reminder to check for parsing failure
			make_word(tok);
			ast = parse(&tok);
			if (ast)
				print_ast(ast, 0);
			clean_ast(ast);
			print_linear_tok(tok, "thing");
			add_history(buf);
		}
		free(buf);
		clean_tok(&tok);
		muh_number = 0;
	}
	(void) c;
	(void) v;
}
