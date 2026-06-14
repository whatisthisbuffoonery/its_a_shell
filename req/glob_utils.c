/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:44 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:44 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	glob_match(char *pattern, char *name)
{
	if (!*pattern && !*name)
		return (1);
	if (*pattern == '*')
		return (glob_match(pattern + 1, name)
			|| (*name && glob_match(pattern, name + 1)));
	if (*pattern != *name)
		return (0);
	return (glob_match(pattern + 1, name + 1));
}

int	glob_append(t_arg **head, t_arg **cur, char *name)
{
	t_arg	*node;

	node = ft_calloc(1, sizeof(t_arg));
	if (!node)
		return (0);
	node->str = ft_strdup(name);
	node->mask = ft_calloc(ft_strlen(name), sizeof(char));
	if (!node->str || !node->mask)
	{
		free_arg(node);
		return (0);
	}
	if (!*head)
		*head = node;
	else
		(*cur)->next = node;
	*cur = node;
	return (1);
}

t_arg	*glob_split(t_arg *fields)
{
	DIR				*dir;
	struct dirent	*entry;
	t_arg			*head;
	t_arg			*cur;

	head = NULL;
	cur = NULL;
	dir = opendir("./");
	if (ft_err(-!dir, "opendir error"))
		return (fields);
	entry = readdir(dir);
	while (entry)
	{
		if (entry->d_name[0] != '.' && glob_match(fields->str, entry->d_name)
			&& !glob_append(&head, &cur, entry->d_name))
		{
			closedir(dir);
			return (free_arg_list(head));
		}
		entry = readdir(dir);
	}
	closedir(dir);
	if (!head)
		return (fields);
	return (head);
}

int	has_glob(t_arg *field)
{
	int	i;

	if (!field || !field->str)
		return (0);
	i = 0;
	while (field->str[i])
	{
		if (field->mask[i] && field->str[i] == '*')
			return (1);
		i++;
	}
	return (0);
}

int	do_glob(t_arg **prev, t_arg **iter, t_arg **next, t_arg **head)
{
	t_arg	*ex;

	ex = glob_split(*iter);//returns iter on no result, so NULL is an error
	if (!ex)
		return (1);
	if (ex == *iter)
		*prev = *iter;
	else
	{
		if (!*prev)
			*head = ex;
		else
			(*prev)->next = ex;
		*prev = ex;
		while ((*prev)->next)
			*prev = (*prev)->next;
		(*prev)->next = *next;
		free_arg(*iter);
	}
	return (0);
}



//-----------------------------------------------------------------------

t_arg	*fake_token(void)
{
	int		i;
	int		len;
	t_arg   *arg;
	t_tok   fake;

	ft_printf("\nfake token testing:\n");
	fake.type = '$';
	fake.str = ft_strdup("  $a$b-hello world  *.c* env*_init*  hi   ");
	fake.word_next = NULL;
	fake.next = NULL;
	ft_printf("tkn(->next)->str: %s\n", fake.str);
	split_expand(&arg, &fake);
	ft_printf("arg->str: %s\n", arg->str);
	len = ft_strlen(arg->str);
	ft_printf("arg->mas: ", arg->mask);
	i = 0;
	while (i < len)
	{
		if (arg->mask[i])
			ft_putchar('1');
		else
			ft_putchar('0');
		i ++;
	}
	ft_putstr("\n\n");
	free(fake.str);
	return (arg);
}
