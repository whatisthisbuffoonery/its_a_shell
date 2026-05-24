/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 18:02:20 by achew             #+#    #+#             */
/*   Updated: 2026/04/28 18:17:07 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	env_builtin(int argc, char **argv, t_env *env, int fd)
{
	(void) argv;
	if (argc != 1)
	{
		ft_putstr_fd("only support env without options and arguments\n", 2);
		return (125);
	}
	print_env(env->env, fd);
	return (0);
}	 
