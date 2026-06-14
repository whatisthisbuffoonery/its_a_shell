/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:47 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:47 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	pwd(t_env *env, int out)
{
	int	pwdlen;

	if (!env->pwd)
		env->pwd = getcwd(NULL, 0);
	if (env->pwd)
	{
		pwdlen = ft_strlen(env->pwd);
		if (env->pwd[pwdlen - 1] == '/')
			env->pwd[pwdlen - 1] = '\0';
		ft_putstr_fd(env->pwd, out);
		ft_putchar_fd('\n', out);
	}
	return (builtin_err(!env->pwd, "pwd", "unable to recover pwd"));
}
