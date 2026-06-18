/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 23:54:31 by achew             #+#    #+#             */
/*   Updated: 2026/06/18 23:54:33 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strndup(const char *src, size_t n)
{
	size_t	i;
	char	*ret;

	i = 0;
	if (ft_strlen(src) < n)
		n = ft_strlen(src);
	ret = malloc(n + 1);
	if (!ret)
		return (NULL);
	while (i < n)
	{
		ret[i] = src[i];
		i ++;
	}
	ret[i] = '\0';
	return (ret);
}
