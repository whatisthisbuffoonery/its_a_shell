/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_chk.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 23:54:16 by achew             #+#    #+#             */
/*   Updated: 2026/06/18 23:54:18 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew_chk(void *content, int (*chk)(void *), void (*del)(void *))
{
	t_list	*a;

	if (chk && chk(content))
		return (NULL);
	a = malloc(sizeof(t_list));
	if (!a)
	{
		if (del)
			del(content);
		return (NULL);
	}
	a->next = NULL;
	a->content = content;
	return (a);
}
