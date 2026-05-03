#include "h_minishell.h"

void	copy_wrapper(char *src, char *dst, int *i, int *len)
{
	if (dst)
		dst[*len] = src[*i];
	*i += 1;
	*len += 1;
}

int	concat_wrapper(t_tok *dst, t_env *env, char *ret, int *i)
{
	char	*src;
	char	*str;
	int		k;
	int		tmp_len;

	k = 0;
	str = &dst->str[*i + 1];
	tmp_len = 0;
	while (isenv(str[k]))//huh
		k ++;
	src = find_env_str(str, env, k);
	if (ret && src)
		ft_strlcat(ret, src, -1);
	if (src)
		tmp_len += ft_strlen(src);
	*i += k + (str[0] == '?') + 1;
	return (tmp_len);
}

int	use_expansion(t_tok *dst, t_env *env, char *ret)
{
	int		i;
	int		len;

	i = 0;
	len = 0;
	if (ret)
		ret[0] = '\0';
	while (dst->str[i])
	{
		if (envname(&dst->str[i]))
			len += concat_wrapper(dst, env, ret, &i);//either strlen or strlcat
		else
			copy_wrapper(dst->str, ret, &i, &len);//copy one char//yes we copy dollar sign if env name is invalid
	}
	if (!ret
		&& (!ft_err(-!malloc_cond((void **) &ret, len + 1), "expansion")))
		return (use_expansion(dst, env, ret));
	else if (!ret)
		return (1);
	free(dst->str);
	dst->str = ret;
	return (0);
}

//todo: mask delim/special, then strjoin, then re tokenise
int	expand_word(t_tok **tok, t_env *env)
{
	t_tok	*iter;

	iter = *tok;
	while (iter)
	{
		if (iter->type != '\'' && use_expansion(iter, env, NULL))
			return (1);
		iter = iter->word_next;
	}
	return (0);
}

t_arg	*arg_init(t_tok *iter)
{
	int		i;
	t_arg	*ret;

	i = 0;
	while (iter)
	{
		i += ft_strlen(iter->str);
		iter = iter->word_next;
	}
	ret = ft_calloc(1, sizeof(t_arg));
	if (ret)
	{
		ret->mask = malloc(i + 1);
		ret->str = malloc(i + 1);
		if (!ret->mask || !ret->str)
		{
			free(ret->mask);
			free(ret->str);
			free(ret);
			ret = NULL;
		}
		else
			ret->str[0] = '\0';
	}
	return (ret);
}
		

//check null token in parent
int	split_expand(t_arg **dst, t_tok *src)
{
	int		i;
	int		len;
	t_tok	*iter;

	*dst = arg_init(src);
	len = 0;
	iter = src;
	while (*dst && iter)
	{
		i = 0;
		while (iter->str && iter->str[i])
		{
			if (!ft_isquote(iter->type)
				&& (ft_isspace(iter->str[i]) || iter->str[i] == '*'))
				(*dst)->mask[len + i] = 1;
			else
				(*dst)->mask[len + i] = 0;
			i ++;
		}
		len += i;
		(*dst)->mask[len] = 0;
		ft_strlcat((*dst)->str, iter->str, -1);
		iter = iter->word_next;
	}
	return (ft_err(-!*dst, "expansion splitting malloc"));
}


t_arg	*fake_token(void)
{
	int		i;
	int		len;
	t_arg   *arg;
	t_tok   fake;

	ft_printf("\nfake token testing:\n");
	fake.type = '$';
	fake.str = ft_strdup("  hello world  *.c   **.o  hi   ");
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
	return (arg);
//	free(arg->mask);
//	free(arg->str);
//	free(arg);
//	free(fake.str);
}

int	expand_all_debug(t_tok **tok, t_env *env)
{
	t_tok	*iter;
	t_arg	*arg;
	int		i;
	int		len;
	
	iter = *tok;
	while (iter)
	{
		if (expand_word(&iter, env))
			return (1);
		iter = iter->next;
	}
	iter = *tok;
	while (iter)
	{
		ft_printf("tkn(->next)->str: %s\n", iter->str);
		split_expand(&arg, iter);
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
		ft_putchar('\n');
		free(arg->mask);
		free(arg->str);
		free(arg);
		iter = iter->next;
	}
	return (0);//malloc check later
}

void	free_arg(t_arg *arg)
{
	free(arg->str);
	free(arg->mask);
	free(arg);
}

t_arg	*free_arg_list(t_arg *head)
{
	t_arg	*next;
	
	while (head)
	{
		next = head->next;
		free_arg(head);
		head = next;
	}
	return (NULL);
}

t_arg	*new_field(t_arg *src, int start, int end)
{
	t_arg	*node;
	int		len;
	char	*str;
	char	*mask;

	str = src->str;
	mask = src->mask;
	len = end - start;
	node = ft_calloc(1, sizeof(t_arg));
	if (!node)
		return (NULL);
	node->str = ft_calloc(len + 1, sizeof(char));
	node->mask = ft_calloc(len + 1, sizeof(char));
	if (!node->str || !node->mask)
	{
		free(node->str);
		free(node->mask);
		free(node);
		return (NULL);
	}
	ft_strlcpy(node->str, &str[start], len + 1);
	ft_memcpy(node->mask, &mask[start], len);
	return (node);
}

t_arg	*append_new_field(t_arg *new, t_arg **head, t_arg **cur)
{
	if (!new)
		return (NULL);
	if (!*head)
		*head = new;
	else
		(*cur)->next = new;
	*cur = new;
	return (new);
}

t_arg	*field_split(t_arg *src)
{
	t_arg	*head;
	t_arg	*cur;
	int		start;	
	int		next_to_1;
	int		i;

	head = NULL;
	cur = NULL;
	i = 0;
	next_to_1 = 0;
	start = 0;
	while (src->str[i])
	{
		if (src->mask[i] && ft_isspace(src->str[i]) && !next_to_1 && i > start)
			if (!(append_new_field(new_field(src, start, i), &head, &cur)))
				return (free_arg_list(head));
		next_to_1 = src->mask[i] && ft_isspace(src->str[i]);
		if (next_to_1)
			start = i + 1;
		i++;
	}
	if (i > start && !(append_new_field(new_field(src, start, i), &head, &cur)))
		return (free_arg_list(head));
	if (!head) //if src->str was empty
		return (src);
	return (head);
}

//void	expand_debug_2(t_tok **tok, t_env *env)
//{
//	char	**argv;
//	t_tok	*iter;
//	t_arg	*arg;
//	int		i;
//	int		k;
//	int		len;
//
//	iter = *tok;
//	while (iter)
//	{
//		if (expand_word(&iter, env))
//			return (1);
//		iter = iter->next;
//	}
//	iter = *tok;
//	while (iter)
//	{
//		split_expand(&arg, iter);
//		k = 0;
//		i = 0;
//		len = 0;
//		while (arg->str[i])
//		{
//			if (!i && !arg->mask[i])
//				k ++;
//			else if (!arg->mask[i] && arg->str[i + 1] && arg->mask[i + 1])
//				k ++;
//			else if (arg->mask[i] && arg->str[i] == '*')
//				k ++;
//			i ++;
//		}
//		argv = ft_calloc(k + 1, sizeof(char *));
//		i = 0;
//		while (arg->str[i])
//		{
//			len = 0;
//			if (arg)
//		}
//	}
//}
