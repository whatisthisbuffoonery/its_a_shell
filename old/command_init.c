/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 17:29:51 by achew             #+#    #+#             */
/*   Updated: 2026/04/18 22:00:38 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

t_command	*create_command(t_cmd *start, t_cmd *end)
{
	t_command	*command;
	int			i;

	command = malloc(sizeof(t_command));
	if (!command)
		return (NULL);
	i = 0;
	while (ft_isspace(start->str[i]))
		i++;
	ft_split_isspace(&start->str[i]);
	command->seq
	command->next = NULL;
	return (command);
}

t_command	*append_command(t_command **head, t_command	*new)
{
	t_command	*iter;

	iter = *head;
	if (!iter)
	{
		*head = new;
		return (head);
	}
	while (iter && iter->next)
		iter = iter->next;
	iter->next = new;
	return (head);
}
void	command_init(t_cmd *cmd, int *fail)
{
	t_command	*new;
	t_cmd		*start;
	t_cmd		*end;

	start = find_start(cmd);
	end = find_end(cmd);
	new = create_command(start, cmd);
	if (!new)
	{
		*fail = ft_err(-1, "command node malloc");
		return ;
	}
	append_command(&head, new);
	new = create_command(cmd->next, end);
	if (!new)
	{
		*fail = ft_err(-1, "command node malloc");
		return ;
	}
	append_command(&head, new);
}

t_command	*commands_init(t_cmd *cmd, int *fail)
{
	t_command	*head;
	
	head = NULL;
	start = cmd;
	while (cmd)
	{
		if (cmd->type == '&' || cmd->type == '|')
				&& ft_strlen(cmd->str == 2)
		{
			command_init(cmd, fail);
			if (fail)
				return (head);
				//wait. how to not find the same commands in next iteration?
			//(ls && ((echo a || ls) || (ls && echo a) && ls) && echo a)
			//(ls && echo a || ls || ls && echo a && ls && echo a)
			//
		}	
		cmd = cmd->next;
	}
	return (head);
}

