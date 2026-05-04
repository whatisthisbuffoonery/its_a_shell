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

//wont really be used outside of debug
int	expand_str(t_tok **tok, t_env *env)
{
	t_tok	*iter;
	iter = *tok;
	while (iter)
	{
		if (expand_word(&iter, env))
			return (1);
		iter = iter->next;
	}
	return (0);
}

//--------------------------------------------------------------------------------

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
		ft_strlcat((*dst)->str, iter->str, -1);
		iter = iter->word_next;
	}
	return (ft_err(-!*dst, "expansion splitting malloc"));
}

t_arg	*new_field(t_arg *src, int start, int end)
{
	t_arg	*node;
	int		len;

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
	ft_strlcpy(node->str, &src->str[start], len + 1);
	ft_memcpy(node->mask, &src->mask[start], len);
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

//for empty word i.e. $c="     " unquoted, this returns src
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
		if (src->mask[i] && ft_isspace(src->str[i]) && !next_to_1 && i > start
			&& !append_new_field(new_field(src, start, i), &head, &cur))
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

//------------------------------------------------------------------------------------------

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
	node->mask = ft_calloc(ft_strlen(name), sizeof(char));//I would set to null
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

t_arg	*glob_expand(t_arg *fields)
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

	ex = glob_expand(*iter);//returns iter on no result, so NULL is an error
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

//src destroyed on failure
t_arg	*expand_globs(t_arg *fields)
{
	t_arg	*iter;
	t_arg	*next;
	t_arg	*prev;
	t_arg	*head;

	head = fields;
	iter = fields;
	prev = NULL;
	while (iter)
	{
		next = iter->next;
		if (has_glob(iter))
		{
			if (do_glob(&prev, &iter, &next, &head))
				return (free_arg_list(head));
		}
		else
			prev = iter;
		iter = next;
	}
	return (head);
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
