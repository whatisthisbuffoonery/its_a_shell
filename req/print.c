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

void	print_cmd(t_cmd **cmd)
{
	t_cmd	*iter;

	iter = *cmd;
	ft_putstr("me\n");
	while (iter)
	{
//		ft_printf("[%s]\n", iter->str);
		print_word(iter);
		if (iter->env)
			print_env(iter->env);
		iter = iter->next;
	}
}

void	shell_print(t_cmd **cmd, char *buf, t_env *env)
{
	int		i = 0;
	int		cry = 0;
	t_node	*node;

	while (buf && buf[i])//cmd_init replaces this
	{
		while (ft_isspace(buf[i]))
			i ++;
		if (!buf[i])
			break ;
		i += node_init(cmd, &buf[i], &cry);
		if (cry)
			return ;
	}
//	if (buf)
//		ft_printf("strlen: %d, i: %d\n", ft_strlen(buf), i);
	if (buf && buf[0])
	{
		expand_str(cmd, env->env);
		print_cmd(cmd, &cry);
		node = parse(*cmd);
		ast_print(node, 0);
	//	cmd_init(buf, cmd);
//		print_cmd(cmd, &cry);
		add_history(buf);
	}
	else if (!buf)
		ft_putstr("NULL");
	ft_putchar('\n');
	free(buf);
}

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
