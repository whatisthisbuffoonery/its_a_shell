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

int	ft_env(int argc, char **argv, t_env *env)
{
	if (argc != 1 || !argv)
	{
		ft_printf("only support env without options and arguments\n");
		return (1);
	}
	print_env(env->env);
	return (0);
}	 
