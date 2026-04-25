#include "h_minishell.h"

char	*env_safe(char *s, char *n)
{
	if (!s)
		return (n);
	return (s);
}

void	print_env(t_shnode *env)
{
	while (env)
	{
		ft_printf("<%s, %s> ", env->name, env_safe(env->str, "NULL"));
		env = env->next;
	}
	ft_putstr("\n\n");
}

void	print_tok(t_tok **tok)
{
	t_tok	*iter;

	iter = *tok;
	while (iter)
	{
//		ft_printf("[%s]\n", iter->str);
		print_word(iter);
		if (iter->env)
			print_env(iter->env);
		iter = iter->next;
		ft_putchar('\n');
	}
}
/*
void	shell_print(t_tok **tok, char *buf, t_env *env)
{
	int		i = 0;
	int		cry = 0;
	t_node	*node;

	if (buf && buf[0])
	{
		expand_str(tok, env->env);
		print_tok(tok, &cry);
		node = parse(*tok);
		expand_str(tok, env->env);//note to move this step after cst init for semantics
//		print_tok(tok, &cry);
		add_history(buf);
	}
	else if (!buf)
		ft_putstr("NULL");
	ft_putchar('\n');
	free(buf);
}
*/
void	env_print(t_env *env)
{
	t_shnode	*iter;

	iter = env->env;
	ft_putstr("\n\nenv list\n\n");
	while (iter)
	{
		ft_printf("%s = ", iter->name);
		if (iter->str)
			ft_putstr(iter->str);
		else
			ft_putstr("NULL");
		ft_putchar('\n');
		iter = iter->next;
	}
	iter = env->export;
	ft_putstr("\n\nexport list\n\n");
	while (iter)
	{
		ft_printf("%s = ", iter->name);
		if (iter->str)
			ft_putstr(iter->str);
		else
			ft_putstr("NULL");
		ft_putchar('\n');
		iter = iter->next;
	}
	ft_putstr("\n\n");
}
