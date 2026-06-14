/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:11 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:11 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	isredir(int c)
{
	return (c == '>' || c == '<');
}

int	isbracket(int c)//move this over to libft
{
	return (c == '(' || c == ')');
}

int	iscond(int c)
{
	return (c == '|' || c == '&');
}

int	isop(int c)
{
	return (isredir(c) || iscond(c));
}

int	isenv(char c)
{
	//ft_printf("%d\n", c);
	return (c == '_' || ft_isalnum(c));
}

int	envname(char *s)
{
	return (s[0] == '$' && (isenv(s[1]) || s[1] == '?'));
}

int	iscontent(int c)
{
	return (c && c != '*' /*&& c != '='  do not consider */
		&& !isop(c) && !ft_isquote(c) && !ft_isspace(c) && !isbracket(c));
}

int	is_assignment_word(char *s) //only when export is argv[0] of the cmd
{
	int	i;

	if (!ft_isalpha(*s) && *s != '_')
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!ft_isalnum(s[i]) && s[i] != '_')
			return (0);
		i++;
	}
	return (s[i] == '=');
}
