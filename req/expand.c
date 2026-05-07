#include "h_minishell.h"

//guaranteed non null
char	**count_str(t_arg *globbed)
{
	int		i;
	char	**ret;
	t_arg	*next;

	i = 0;
	next = globbed;
	while(next)
	{
		i ++;
		next = next->next;
	}
	ret = malloc((i + 1) * sizeof(char *));
	if (ft_err(-!ret, "malloc error"))
		return (NULL);
	i = 0;
	while (globbed)
	{
		next = globbed->next;
		ret[i++] = globbed->str;
		free(globbed->mask);
		free(globbed);
		globbed = next;
	}
	ret[i] = NULL;
	return (ret);
}

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
		fields = field_split(arg);
		if (fields != arg)
			free_arg(arg);
		if (!fields)
		{
			free_arg_list(head);
			return (1);
		}
		append_new_wrapper(&head, &tail, fields);
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

//to do: verify malloc failure detection and find somewhere to stick err msg
char	**expand_all(t_tok *src, t_env *env, int (*f)(t_arg **, t_tok *))
{
	t_arg	*globbed;
	char	**ret;

	if (expand_str(src, env) || f(&globbed, src))
		return (NULL);
	ret = count_str(globbed);
	if (!ret)
		free_arg_list(globbed);
	return (ret);
}
