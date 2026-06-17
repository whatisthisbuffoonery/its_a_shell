/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 18:31:32 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/17 18:31:33 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

void	path_error(char *new, int *status)
{
	struct stat	dump;

	if (errno == ENOENT || stat(new, &dump))
		return ;
	ft_err(-1, new);
	*status = errno;
	(void) dump;
}

//calling access("dir/ + empty string", X_OK) queries the dir itself
//can return 0, not gud
int	empty_path(char *dst, char *new, int *status)
{
	if (dst[0])
		return (0);
	free(new);
	*status = ENOENT;
	return (1);
}

void	find_path(char *path, char **dst, char *new, int *status)
{
	int		i;
	int		k;

	i = 0;
	if (empty_path(*dst, new, status))
		return ;
	while (path[i])
	{
		k = 0;
		while (path[i + k] && path[i + k] != ':')
			k ++;
		ft_strlcpy(new, &path[i], k + 1);
		ft_strlcat(new, "/", -1);
		ft_strlcat(new, *dst, -1);
		if (!access(new, X_OK))
		{
			*status = 0;
			free(*dst);
			*dst = new;
			return ;
		}
		path_error(new, status);
		i += k + (path[i + k] != '\0');
	}
	free(new);
}
