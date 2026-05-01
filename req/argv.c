#include "h_minishell.h"

int	isbuiltin(char *s)
{
	return (!ft_strcmp(s, "env") || !ft_strcmp(s, "cd") || !ft_strcmp(s, "pwd")
		|| !ft_strcmp(s, "echo") || !ft_strcmp(s, "export")
		|| !ft_strcmp(s, "unset") || !ft_strcmp(s, "exit"));
}

int	do_builtin_match(int argc, char **argv, t_env *env)
{
	if ((*argv)[0] == 'c')//cd
		;
	else if ((*argv)[0] == 'u')//unset
		;
	else if ((*argv)[0] == 'p')//pwd
		;
	else if ((*argv)[1] == 'n')//env
		;
	else if ((*argv)[1] == 'c')//echo
		;
	else if ((*argv)[2] == 'p')//export
		;
	else if ((*argv)[2] == 'i')//exit
		;
}

char	*longest_argv(char *path, char *src)
{
	char	*ret;
	int		max;
	int		i;
	int		k;

	max = 0;
	i = 0;//no PATH= offset
	while (path[i])
	{
		k = 0;
		while (path[i + k] && path[i + k] != ':')
			k ++;
		if (k > max)
			max = k;
		i += k + (path[i + k] != '\0');
	}
	ret = malloc(max + ft_strlen(src) + 1 + 1);//extra forward slash
	return (ret);
}

void	find_path(char *path, char **dst, char *new)
{
	int		i;
	int		k;

	i = 0;
	while (path[i])
	{
		k = 0;
		while (path[i + k] && path[i + k] != ':')
			k ++;
		ft_strlcpy(new, &path[i], k + 1);
		ft_strlcat(new, "/", -1);
		ft_strlcat(new, *dst, -1);
		if (!access(new, X_OK))
		{
			free(*dst);
			*dst = new;
			return ;
		}
		i += k + (path[i + k] != '\0');
	}
	free(new);
}

//recontruct
//will ironically be called from redir_to_fd
char    **make_argv(t_tok *src, t_env *env)
{
    char		**argv;
	char		*cmd;
	t_shnode	*path;

	argv = expand_env(src->argv);//no need double ptr//uhh... she handles glob? half half lah hor//take tok for arg
	if (!argv)
		return (NULL);
	path = find_env("PATH", env->env, ft_strlen("PATH"));
	if (ft_strchr(*argv, '/') || isbuiltin(*argv)
		|| !path || !path->str || !path->str[0])
		return (argv);
	cmd = longest_path(path->str, *argv);//find the largest env string and use that size
	if (ft_err(-!cmd, "cmd malloc error"))
		return (split_cleanup(argv));
	find_path(env, argv, cmd);//free unused string
	return (argv);
}
