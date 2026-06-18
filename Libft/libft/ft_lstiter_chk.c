/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_chk.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 23:53:49 by achew             #+#    #+#             */
/*   Updated: 2026/06/18 23:53:52 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstiter_chk(t_list *lst, void (*f)(void *), int (*chk)(void *))
{
	if (!f || !lst)
		return (1);
	while (lst)
	{
		f(lst->content);
		if (chk && chk(lst->content))
			return (1);
		lst = lst->next;
	}
	return (0);
}
