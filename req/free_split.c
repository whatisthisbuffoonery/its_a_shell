/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_split.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 20:01:16 by achew             #+#    #+#             */
/*   Updated: 2026/04/23 20:02:23 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

void free_split(char **split)
{
    int i;
	
	i = 0;
    if (!split)
        return ;
    while (split[i])
    {
        free(split[i]); 
        i++;
    }
    free(split);
}

