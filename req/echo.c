/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 18:37:02 by achew             #+#    #+#             */
/*   Updated: 2026/04/28 18:56:51 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	ft_echo(int argc, char **argv, t_env *env)
{
	int	i;
	int	n_flag;

	if (argc < 2)
	{
		ft_printf("\n");
		return (0);
	}
	n_flag = 0;
	i = 1;
	if (!ft_strcmp(argv[1], "-n"))
	{
		n_flag = 1;
		i++;
	}
	while (argv[i])
		ft_printf("%s ", argv[i++]);
	if (!n_flag)
		ft_printf("\n");
	(void)env;
	return (0);
}
