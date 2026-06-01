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

static int	invalid_identifier(const char *arg)
{
	write(2, "export: `", 9);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 27);
	return (1);
}

static int	  is_in_list(const char *name, t_shnode *list)
{
	return (find_env(name, list));
}

static int	update_val(const char *val, t_shnode *existing)
{
	char	*new_str;

	new_str = ft_strdup(val);
	if (!new_str)
		return (1);
	free(existing->str);
	existing->str = new_str;
	return (0);
}

static int	  export_set(t_env *env, const char *name, const char *val)
{
	t_shnode	*existing;
	t_shnode	*node;
	t_shnode	*node2;
	char		*entry;

	existing = is_in_list(name, env->export);
	if (existing && val)
		return (update_val(val, existing, n, env));
	else if (existing)
		return (0);
	node = env_init_node(name);
	if (!node)
		return (1);
	env_add(env, node, "export");
	if (val)
	{
		node2 = shnode_dup(node);
		if (!node2)
			return (1);
		env_add(env, node2, "env");
	}
	return (0);
}

static int	process_export_arg(const char *arg, t_env *env)
{
	char		*eq;
	char		*name;

	if (!arg || arg[0] == '=')
		return (invalid_identifier(arg));
	eq = ft_strchr(arg, '=');
	if (eq)
		ft_strlcpy(name, arg, eq - arg);
	else
		name = arg;
	if (invalid_var(name))
		return (invalid_identifier(name));
	if (!eq)
		return (export_set(env, name, NULL));
	else
		return (export_set(env, name, eq + 1));
}

int    ft_export(int argc, char **argv, t_env *env)
{
	int		i;
	int		status;

	if (argc < 2)
	{
		print_export(env->export);
		return (0);
	}
	status = 0;
	i = 1;
	while (i < argc)
	{
		status = process_export_arg(argv[i], env);
		i++;
	}
	return (status);
}
