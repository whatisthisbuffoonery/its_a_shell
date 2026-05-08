/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:36:31 by achew             #+#    #+#             */
/*   Updated: 2026/05/08 17:37:54 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

void	relink(t_shnode *head, t_shnode *prev, t_shnode *next)
{
	t_shnode	**add;

	add = &head;
	if (!prev)
		*add = next;
	else
		prev->next = next;
}


t_shnode	*find_prev(t_shnode *list, char *var)
{
	while (list && list->next)
	{
		if (!ft_strcmp(list->next->name, var))
			return (list);
		list = list->next;
	}
	return (NULL);
}	

int	ft_unset(int argc, char **argv, t_env *env)
{
	t_shnode	*existing_env;
	t_shnode	*existing_exp;
	int			i;

	if (argc < 2)
		return (0);
	i = 1;
	while (argv[i])
	{
		existing_exp = find_env((char *)argv[i], env->export, ft_strlen(argv[i]));
		if (existing_exp)
		{
			relink(env->export, find_prev(env->export, argv[i]), existing_exp->next);
			shnode_free(existing_exp);
		}
		existing_env = find_env((char *)argv[i], env->env, ft_strlen(argv[i]));
		if (existing_env)
		{
			relink(env->env, find_prev(env->env, argv[i]), existing_env->next);
			shnode_free(existing_env);
		}
		i++;
	}
	return (0);
}
