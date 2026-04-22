/* ************************************************************************** */
/*																			  */
/*														  :::	   ::::::::   */
/*	 export.c											:+:		 :+:	:+:   */
/*													  +:+ +:+		  +:+	  */
/*	 By: achew <achew@student.42singapore.sg>		+#+  +:+	   +#+		  */
/*												  +#+#+#+#+#+	+#+			  */
/*	 Created: 2026/04/22 01:12:45 by achew			   #+#	  #+#			  */
/*	 Updated: 2026/04/22 01:12:53 by achew			  ###	########.fr		  */
/*																			  */
/* ************************************************************************** */

#include "h_minishell.h"

void    shnode_free(t_shnode *node)
{
    if (!node)
        return ;
    free(node->name);
    free(node->str);
    free(node);
}

char	*ft_strjoin3(const char *name, const char *val)
{
	char    *entry;
    size_t  len;

	if (!val)
		val = "";
    len = ft_strlen(name) + ft_strlen(val) + 2;
    entry = malloc(len);
    if (!entry)
        return (NULL);
    ft_strlcpy(entry, name, len);
    ft_strlcat(entry, "=", len);
    ft_strlcat(entry, val ? val : "", len);
    return (entry);
}

int		invalid_var(const char *var)
{
	int		i;

	if (!var || !var[0])
		return (1);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (1);
	i = 1;
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
			return (1);
		i++;
	}
	return (0);
}

void	invalid_identifier(const char *arg)
{
	write(2, "export: `", 9);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 25);
}

static int	  is_exported(t_env *env, const char *name)
{
	return (find_env((char *)name, env->export, ft_strlen(name)) != NULL);
}

static char    *get_shell_var(t_env *env, const char *name)
{
	t_shnode	*node;

	node = find_env((char *)name, env->env, ft_strlen(name));
	if (!node)
		return (NULL);
	return (node->str);
}

int	update_val(const char *name, const char *val, t_shnode *existing, t_env *env)
{
	char	*new_str;

	if (!val)
		new_str = NULL;
	else
		new_str = ft_strdup(val);
	if (val && !new_str)
		return (-1);
	free(existing->str);
	existing->str = new_str;
	existing = find_env((char *)name, env->env, ft_strlen(name));
	if (existing)
	{
		free(existing->str);
		if (!val)
			new_str = NULL;
		else
			new_str = ft_strdup(val);
		existing->str = new_str;
	}
	return (0);
}

static int	  export_set(t_env *env, const char *name, const char *val)
{
	t_shnode	*existing;
	t_shnode	*node;
	char		*entry;

	existing = find_env((char *)name, env->export, ft_strlen(name));
	if (existing)
		return (update_val(name, val, existing, env));
	if (val)
		entry = ft_strjoin3(name, val);
	else
		entry = ft_strjoin3(name, "");
	if (!entry)
		return (-1);
	node = env_init_node(entry);
	free(entry);
	if (!node)
		return (-1);
	env_add(env, node, "export");
	env_add(env, node, "env");
	return (0);
}

static int	process_export_arg(const char *arg, t_env *env)
{
	char		*eq;
	t_shnode	*node;

	if (!arg)
		return (1);
	if (arg[0] == '=')
		return (invalid_identifier(arg), 1);
	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (invalid_var(arg))
			return (invalid_identifier(arg), 1);
		if (!is_exported(env, arg))
			return (export_set(env, arg, get_shell_var(env, arg)));
		return (0);
	}
	node = env_init_node((char *)arg);
	if (!node || invalid_var(node->name))
	{
		invalid_identifier(arg);
		return (shnode_free(node), 1);
	}
//	shnode_free(node);
	return (export_set(env, node->name, node->str));
}

int    ft_export(int argc, char **argv, t_env *env)
{
	int		i;
	int		status;

	if (argc < 2)
	{
		ft_printf("export without args technically not specified\n");
		return (0);
	}
	status = 0;
	i = 1;
	while (i < argc)
	{
		if (process_export_arg(argv[i], env) != 0)
			status = 1;
		i++;
	}
	return (status);
}
