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
	while (buf && buf[i] && ft_isspace(buf[i]))
		i ++;
	return (!buf || !buf[i]);
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
	ft_memset(env->last_string, 0, 4);
	env->last_string[0] = '0';
}

void	print_word_next(t_tok *tok)
{
	int	i;
	int	end_space;

	printf("tok->word_next list:\n");
	i = 0;
	while (tok)
	{
		printf("  %d. str: %s\n", i, tok->str);
		printf("  %d. type: %c\n", i, tok->type);
		end_space = 0;
		if (tok->end_space == 1)
			end_space = 1;
		printf("  %d. end_space: %d\n", i, end_space);
		if (tok->next)
			printf("  %d. next: %s\n", i, tok->next->str);
		else
			printf("  %d. next: null\n", i);
		printf("\n");
		tok = tok->word_next;
		i++;
	}
}
void	print_next(t_tok *tok)
{
	int	i;
	int	end_space;

	printf("tok->next list:\n");
	i = 0;
	while (tok)
	{
		printf("%d. str: %s\n", i, tok->str);
		printf("%d. type: %c\n", i, tok->type);
		end_space = 0;
		if (tok->end_space == 1)
			end_space = 1;
		printf("%d. end_space: %d\n", i, end_space);
		if (tok->word_next)
		{
			printf("%d. word_next: not null\n", i);
			print_word_next(tok->word_next);
		}
		else
			printf("%d. word_next: null\n", i);
		printf("\n");
		tok = tok->next;
		i++;
	}
}

void	print_fields(t_arg *arg)
{
	int	i;
	int	len;
	int	j;

	j = 0;
	while (arg)
	{
		ft_printf("%d: arg->str: %s\n", j, arg->str);
		ft_printf("%d: arg->mas: ", j);
		len = ft_strlen(arg->str);
		i = 0;
        while (i < len)
        {
            if (arg->mask[i])
                ft_putchar('1');
            else
                ft_putchar('0');
            i ++;
        }
		ft_printf("\n");
		j++;
		arg = arg->next;
	}
	ft_printf("\n");
}

//need to update last in execution
int	execute_buffer(t_env *env, t_tok **tok)
{
	t_arg	*arg;
	t_arg	*fields;
	t_arg	*globbed;

	make_word(*tok);
	//print_next(*tok);
	//expand_all_debug(tok, env);
	//arg = fake_token();
	expand_str(tok, env);
	split_expand(&arg, *tok);
	fields = field_split(arg);
	if (fields != arg)
		free_arg(arg);
	print_fields(fields);
	globbed = expand_globs(fields);
	ft_putstr("\n===final===\n");
	print_fields(globbed);
	free_arg_list(globbed);
/*	env->ast = parse(tok);
	if (env->ast)
		print_ast(env->ast, 0);
	clean_ast(env->ast);
	env->ast = NULL;*/
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
		if (buf[0])
			add_history(buf);//different rule
		if (!isempty(buf))
		{
			env.last = tok_init(buf, &tok);
			free(buf);
			buf = NULL;
			if (!env.last)
				execute_buffer(&env, &tok);
		//	print_linear_tok(tok, "tok chain: ");
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
