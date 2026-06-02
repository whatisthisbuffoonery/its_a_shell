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

int		invalid_var(char *var)
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

static int	invalid_identifier(char *arg)
{
	write(2, "export: `", 9);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 27);
	return (1);
}

static t_shnode	*is_in_list(char *name, t_shnode *list)
{
	return (find_env(name, list));
}

static int	update_val(char *val, t_shnode *existing)
{
	char	*new_str;

	new_str = ft_strdup(val);
	if (!new_str)
		return (1);
	free(existing->str);
	existing->str = new_str;
	return (0);
}

static int	env_set(t_shnode *exp_node, char *name, char *val, t_env *env)
{
	t_shnode	*existing;
	t_shnode	*env_node;

	existing = is_in_list(name, env->env);
	if (existing && val)
		existing->str = exp_node->str;
	else if (val)
	{
		env_node = shnode_dup(exp_node);
		if (!env_node)
			return (1);
		shnode_append(&env->env, env_node);
	}
	return (0);
}

static int	  export_set(char *arg, char *name, char *val, t_env *env)
{
	t_shnode	*existing;
	t_shnode	*exp_node;

	existing = is_in_list(name, env->export);
	exp_node = existing;
	if (existing && val)
		update_val(val, existing);
	else if (!existing)
	{
		exp_node = env_init_node(arg);
		if (!exp_node)
			return (1);
		shnode_append(&env->export, exp_node);
	}
	return (env_set(exp_node, name, val, env));
}

static int	process_export_arg2(char *arg, int name_len, char *eq, t_env *env)
{
	char	name[name_len];

	ft_strlcpy(name, arg, name_len + 1);
	if (invalid_var(name))
		return (invalid_identifier(name));
	if (!eq)
		return (export_set(arg, name, NULL, env));
	else
		return (export_set(arg, name, eq + 1, env));
}

static int	process_export_arg(char *arg, t_env *env)
{
	char	*eq;
	int		name_len;

	if (!arg || arg[0] == '=')
		return (invalid_identifier(arg));
	eq = ft_strchr(arg, '=');
	if (eq)
		name_len = eq - arg;
	else
		name_len = ft_strlen(arg);
	return (process_export_arg2(arg, name_len, eq, env));
}

void    print_export(t_shnode *export, int out)
{
    while (export)
    {   
        ft_putstr_fd("declare -x ", out);
        ft_putstr_fd(export->name, out);
        if (export->str)
		{
			ft_putstr_fd("=\"", out);
        	ft_putstr_fd(export->str, out);
			ft_putstr_fd("\"", out);
		}
		ft_putstr_fd("\n", out);
        export = export->next;
    }   
}

int    ft_export(int argc, char **argv, t_env *env, int out)
{
	int		i;
	int		status;

	if (argc < 2)
	{
		print_export(env->export, out);
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
