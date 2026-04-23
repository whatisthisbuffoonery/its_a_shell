/* ************************************************************************** */
/*																			  */
/*														  :::	   ::::::::   */
/*	 envp.c												:+:		 :+:	:+:   */
/*													  +:+ +:+		  +:+	  */
/*	 By: achew <achew@student.42singapore.sg>		+#+  +:+	   +#+		  */
/*												  +#+#+#+#+#+	+#+			  */
/*	 Created: 2026/04/23 19:24:28 by achew			   #+#	  #+#			  */
/*	 Updated: 2026/04/23 19:24:28 by achew			  ###	########.fr		  */
/*																			  */
/* ************************************************************************** */

#include "h_minishell.h"

char	**env_to_envp(t_shnode *list)
{
	t_shnode	*cur;
	char		**arr;
	char		*entry;
	int			count;
	int			i;

	count = 0;
	cur = list;
	while (cur)
	{
		count++;
		cur = cur->next;
	}
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	cur = list;
	i = 0;
	while (cur)
	{
		entry = ft_strjoin3(cur->name, cur->str);
		if (!entry)
		{
			while (i > 0)
				free(arr[--i]);
			free(arr);
			return (NULL);
		}
		arr[i++] = entry;
		cur = cur->next;
	}
	arr[i] = NULL;
	return (arr);
}
