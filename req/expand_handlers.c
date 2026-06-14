/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_handlers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:34 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:34 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

//empty src is easy to remove at this stage
void	append_new_wrapper(t_arg **head, t_arg **tail, t_arg *src)
{
	int	i;

	i = 0;
	while (!src->next && src->str[i])
	{
		if (!ft_isspace(src->str[i]) || !src->mask[i])
			break ;
		i ++;
	}
	if (!src->next && !src->str[i])
		free_arg(src);
	else
		append_new_field(src, head, tail);
	while (*tail && (*tail)->next)
		*tail = (*tail)->next;
}

//this will be passed as a handler to expand_all
int	collect_argv(t_arg **dst, t_tok *src)
{
	t_arg	*arg;
	t_arg	*fields;
	t_arg	*tail;
	t_arg	*head;

	tail = NULL;
	head = NULL;
	while (src)
	{
		if (split_expand(&arg, src))//always overwrites arg
			return (1);
		if (src->assignment)
		{
			fields = arg;
			fields->next = NULL;
		}
		else
			fields = field_split(arg);
		if (fields != arg)
			free_arg(arg);
		if (!fields)
		{
			free_arg_list(head);
			return (1);
		}
		if (src->assignment)
			append_new_field(fields, &head, &tail);
		else	
			append_new_wrapper(&head, &tail, fields);
		src = src->next;
	}
	*dst = expand_globs(head);//always invalidates src
	return (!*dst);
}

int	collect_redir(t_arg **dst, t_tok *src)
{
	t_arg	*arg;
	t_arg	*fields;

	if (split_expand(&arg, src))
		return (1);
	fields = field_split(arg);
	if (fields != arg)
		free_arg(arg);
	*dst = expand_globs(fields);//does check for null src
	return (!*dst);
}
