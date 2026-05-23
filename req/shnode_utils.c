#include "h_minishell.h"

int	shnode_strlen(t_shnode *env)
{
	if (env && env->str)
		return (ft_strlen(env->str));
	return (0);
}

static t_shnode	*find_env_internal(char *str, t_shnode *list, unsigned int n)
{
	while (list && (ft_strlen(list->name) != n || ft_strncmp(str, list->name, n)))
		list = list->next;
	return (list);
}

t_shnode	*find_env(char *str, t_shnode *list)
{
	size_t	matchlen;

	matchlen = ft_strlen(str);
	while (list && (ft_strlen(list->name) != matchlen
		|| ft_strncmp(str, list->name, matchlen)))
		list = list->next;
	return (list);
}

//turn status assignment into a func ig
char	*find_env_str(char *name, t_env *env, unsigned int len)
{
	t_shnode	*ret;

	ft_printf("env str: %s: %d\n", name, len);
	if (!name || !name[0])
		return (NULL);
	else if (name[0] == '?')
		return (env->last_string);
	else if (len)
	{
		ret = find_env_internal(name, env->env, len);
		if (ret)
			return (ret->str);
	}
	return (NULL);
}

t_shnode	*shnode_dup(t_shnode *src)
{
	t_shnode	*ret;

	if (!src)
		return (NULL);
	ret = malloc(sizeof(t_shnode));
	if (ft_err(-!ret, "shnode dup malloc"))
		return (NULL);
	ret->name = src->name;
	ret->str = src->str;
	ret->next = NULL;
	return (ret);
}

void	shnode_append(t_shnode **dst, t_shnode *src)
{
	t_shnode	*iter;

	iter = *dst;
	while (iter && iter->next)
		iter = iter->next;
	if (iter)
		iter->next = src;
	else
		*dst = src;
}

int	env_add(t_env *env, t_shnode *src, char *dst)
{
	t_shnode	*ret;
	t_shnode	**list;

	ret = src;
	list = &env->export;
	if (dst[1] == 'n')
	{
		ret = shnode_dup(src);
		list = &env->env;
	}
	if (ret)
		ret->next = NULL;
	shnode_append(list, ret);
	return (0);
}
