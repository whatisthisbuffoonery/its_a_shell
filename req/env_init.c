#include "h_minishell.h"

void	clean_one_shnode(t_shnode *node)
{
	free(node->name);
	free(node->str);
	free(node);
}

t_shnode	*env_init_node(char *e)
{
	t_shnode	*ret;
	int			i;

	ret = ft_calloc(1, sizeof(t_shnode)); //initialize with calloc
	if (!ret)
		return (NULL);
	i = 0;
	while (e[i] && e[i] != '=')
		i ++;
	ret->name = ft_strndup(e, i);
	if (!ret->name)
		return (clean_one_shnode(ret), NULL);
	if (e[i]) //bc i need "export a" to be displayed differently from "export a=" (a vs a="")
	{
		ret->str = ft_strdup(&e[i + 1]);
		if (!ret->str)
			return (clean_one_shnode(ret), NULL);
	}
	else //for the above reason
		ret->str = NULL;
	return (ret);
}

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

//SHOULD RUN THIS in case user tries to unset shell name
//technically you would getcwd() here to list the binary name and location
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

//init shell level, only init cd dash if null/not present
//also also change SHELL to be minishell, update SHLVL
//dealing with LINES and COLUMNS for display stuff is way outside subject scope
void	env_init(t_env *dst, char **e)
{
	int			i;
	t_shnode	*iter;

	i = 0;
	ft_memset(dst, 0, sizeof(t_env));
	if (!e || !*e)
	{
		ft_putstr_fd("minishell: null envp at init\n", 2);//test program on null envp
		return ;
	}
	while (e[i])
	{
		iter = env_init_node(e[i]);
		if (!iter && ft_err((-!iter), "export node malloc"))
			break ;
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		i ++;
	}
	merge_sort(&dst->export);
	update_shell_lvl(dst);
	update_shell_name(dst);
}
