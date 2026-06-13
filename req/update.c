#include "h_minishell.h"

//note: this func should only be called on making another child within minishell, bash hands off an updated shell lvl already
int	update_shell_lvl(t_env *dst)
{
	t_shnode	*iter;
	char		*ret;
	char		*name;

	name = "SHLVL";
	iter = find_env(name, dst->export);
	if (!iter)
	{
		iter = env_init_node("SHLVL=1");
		if (!iter)
			return (ft_err(-1, "could not replace missing shlvl"));
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		return (0);
	}
	if (iter->str)
		ret = ft_itoa(ft_atoi(iter->str) + 1);
	else
		ret = ft_itoa(1);
	if (!ret)
		return (ft_err(-1, "shlvl update error"));
	free(iter->str);
	iter->str = ret;
	iter = find_env(name, dst->env);
	if (!shell_assert(!iter, "shlvl missing in env"))
		iter->str = ret;
	return (0);
}

int	update_shell_name(t_env *dst)
{
	t_shnode	*iter;
	char		*ret;
	char		*name;

	name = "SHELL";
	iter = find_env(name, dst->export);
	if (!iter)
	{
		iter = env_init_node("SHELL=minishell");
		if (!iter)
			return (ft_err(-1, "could not replace missing shell name"));
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		return (0);
	}
	ret = ft_strdup("minishell");
	if (!ret)
		return (ft_err(-1, "shell name update error"));
	free(iter->str);
	iter->str = ret;
	iter = find_env(name, dst->env);
	if (!shell_assert(!iter, "shell name missing in env"))
		iter->str = ret;
	return (0);
}
