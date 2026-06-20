/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 21:20:19 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/19 21:20:22 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	cd_exit(int status, char *argv, char *newpwd)
{
	if (!status && argv && !ft_strcmp(argv, "-") && newpwd)
	{
		ft_putstr(newpwd);
		ft_putchar('\n');
	}
	return (status);
}
