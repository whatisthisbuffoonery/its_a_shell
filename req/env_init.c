/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:23 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:23 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

t_shnode	*env_init_node(char *e)
{
	t_shnode	*ret;
	int			i;

	ret = ft_calloc(1, sizeof(t_shnode));
	if (!ret)
		return (NULL);
	i = 0;
	while (e[i] && e[i] != '=')
		i ++;
	ret->name = ft_strndup(e, i);
	if (!ret->name)
		return (clean_one_shnode(ret));
	if (e[i]) //bc i need "export a" to be displayed differently from "export a=" (a vs a="")
	{
		ret->str = ft_strdup(&e[i + 1]);
		if (!ret->str)
			return (clean_one_shnode(ret));
	}
	else //for the above reason
		ret->str = NULL;
	return (ret);
}

char	*grab_home(t_env *env)
{
	t_shnode	*tmp;

	tmp = find_env("HOME", env->env);
	if (!tmp)
		return (NULL);//no recovery method
	return (tmp->str);
}

void	pwd_init(t_env *env)
{
	t_shnode	*tmp;

	tmp = find_env("PWD", env->env);
	if (tmp && tmp->str)
		env->pwd = ft_strdup(tmp->str);
	else
		env->pwd = getcwd(NULL, 0);
	tmp = find_env("OLDPWD", env->env);
	if (tmp && tmp->str)
		env->oldpwd = ft_strdup(tmp->str);
	else
		env->oldpwd = ft_strdup(grab_home(env));
	ft_err(-!env->pwd, "pwd init error");
	ft_err(-!env->oldpwd, "oldpwd init error");
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
	update_shell_lvl(dst, 0);
	update_shell_name(dst);
	pwd_init(dst);
}
