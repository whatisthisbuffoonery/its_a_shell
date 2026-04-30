#include "h_minishell.h"

char	*longest_argv(env, char *src)
{
	t_shnode	*iter;
	char		*ret;
	int			max;
	int			len;

	max = 0;
	iter = env->env;//ofc its wring you have to split path first
	while (iter)
	{
		len = 0;
		if (iter->str)
			len = ft_strlen(iter->str);
		if (len > max)
			max = len;
		iter = iter->next;
	}
	ret = NULL;
	if (max)
	{
		ret = malloc(max + ft_strlen(src) + 1);
		ft_err(-!ret, "cmd malloc error");
	}
//	if (ret)
//		ret[0] = '\0';
	return (ret);
}

//recontruct
//will ironically be called from redir_to_fd
char    **make_argv(t_tok *src, t_env *env)
{
    char    **argv`;
	char	*cmd;
    int     start;
    int     i;

	argv = expand_env(src);//no need double ptr//uhh... she handles glob? half half lah hor
	if (!argv)
		return (NULL);
	cmd = *argv;
	if (ft_strchr(cmd, '/'))
		return (argv);
	cmd = longest_path(env, *argv);//find the largest env string and use that size
	if (!cmd)
		return (split_cleanup(argv));
	find_path(env, argv, cmd);//free unused string
	return (ret);
}
