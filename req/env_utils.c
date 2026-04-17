#include "h_minishell.h"

int add_expansion(t_cmd *dst, t_shnode *env, int *index)
{
	t_shnode	*ret;
	char		*str;
	int			i;

	i = 0;
	str = &dst->str[*index + 1];//dollar offset
	while (isenv(str[i]))
		i ++;
//	ft_printf("adding... index: %d, i: %d\n", *index, i);
	*index += i + 1;//use env name len plus dollar
	env = find_env(str, env, i);
	if (find_env(str, dst->env, i) || !env)
		return (0);
	ret = shnode_dup(env);//mallocs a node, does not malloc the strings so dont free those
	if (!ret)
		return (1);
	shnode_append(&dst->env, ret);
	return (0);
}

void	copy_wrapper(char *src, char *dst, int *i, int *len)
{
	if (dst)
		dst[*len] = src[*i];
	*i += 1;
	*len += 1;
}

void	concat_wrapper(t_cmd *dst, char *ret, int *i, int *len)
{
	t_shnode	*iter;
	char		*str;
	int			k;
	int			tmp_len;

	k = 0;
	iter = dst->env;
	str = &dst->str[*i + 1];
	tmp_len = 0;
	while (isenv(str[k]))//huh
		k ++;
	iter = find_env(str, dst->env, k);
	if (ret && iter)
		ft_strlcat(ret, iter->str, -1);
	if (iter)
		tmp_len += ft_strlen(iter->str);
	*i += k + 1;
	*len += tmp_len;
//	ft_printf("\n\nk: %d, tmp_len: %d\n\n", k, tmp_len);
}

int	use_expansion(t_cmd *dst, char *ret)
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
			concat_wrapper(dst, ret, &i, &len);//either strlen or strlcat
		else if (dst->str[i]
			&& !envname(&dst->str[i]))
			copy_wrapper(dst->str, ret, &i, &len);//copy one char//yes we copy dollar sign if env name is invalid
	}
//	ft_printf("\nis null: %d, len: %d\n", !ret, len);
	if (!ret
		&& (!ft_err(-!malloc_cond((void **) &ret, len + 1), "expansion result malloc")))
		return (use_expansion(dst, ret));
	else if (!ret)
		return (1);
	free(dst->str);
	dst->str = ret;
	return (0);
}

int	expand_str(t_cmd **cmd, t_shnode *env)
{
	t_cmd	*iter;
	int		i;
	int		flag;

	iter = *cmd;
	while(iter)
	{
		i = 0;
		while (iter->str[i] && iter->type != '\'')
		{
			//ft_printf("i here: %d\n", i);
			if (envname(&iter->str[i]) && add_expansion(iter, env, &i))
				return (1);
			flag = (iter->str[i] && !envname(&iter->str[i]));
			//ft_printf("i: %d, flag: %d\n", i, flag);
			i += flag;
		}
		iter = iter->next;
	}
	iter = *cmd;
	while (iter)//move this over to command forking side, expand envs before each command, not before *all* commands
	{
		if (iter->type != '\'' && iter->env && use_expansion(iter, NULL))
			return (1);
		iter = iter->next;
	}
	return (0);
}
