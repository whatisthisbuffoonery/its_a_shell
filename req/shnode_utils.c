#include "h_minishell.h"

int	shnode_strlen(t_shnode *env)
{
	if (env && env->str)
		return (ft_strlen(env->str));
	return (0);
}

t_shnode	*find_env(char *str, t_shnode *list, int len)
{
	unsigned int	n;

	n = (unsigned int) len;
	while (list && (ft_strlen(list->name) != n || ft_strncmp(str, list->name, n)))
		list = list->next;
	return (list);
}

t_shnode	*shnode_dup(t_shnode *src)
{
	t_shnode	*ret;

	if (!src)
		return (NULL);
	ret = malloc(sizeof(t_shnode));
	if (!ret)
	{
		ft_err(-1, "shnode dup malloc");
		return (NULL);
	}
	ret->name = ft_strdup(src->name);
	if (!ret->name)
	{
		ft_err(-1, "shnode dup malloc");
		free(ret);
		return (NULL);
	}
	if (src->str)
		ret->str = ft_strdup(src->str);
	else
		ret->str = NULL;
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
