#include "h_minishell.h"

int	isbuiltin(char *s)
{
	return (!ft_strcmp(s, "env") || !ft_strcmp(s, "cd") || !ft_strcmp(s, "pwd")
		|| !ft_strcmp(s, "echo") || !ft_strcmp(s, "export")
		|| !ft_strcmp(s, "unset") || !ft_strcmp(s, "exit"));
}

int	do_builtin_match(int argc, char **argv, t_env *env, int *fd)
{
	int	status;

	status = 0;
	if ((*argv)[0] == 'c')//cd
		status = cd(argc, argv, env);
	else if ((*argv)[0] == 'u')//unset
		status = unset_builtin(argc, argv, env);
	else if ((*argv)[0] == 'p')//pwd
		status = pwd(env, fd[1]);
	else if ((*argv)[1] == 'n')//env
		status = env_builtin(argc, argv, env, fd[1]);
	else if ((*argv)[1] == 'c')//echo
		status = echo(argv, fd[1]);
	else if ((*argv)[2] == 'p')//export
		status = export(argc, argv, env, fd[1]);
	else if ((*argv)[2] == 'i')//exit
		exit_builtin(argc, argv, env, fd);//uses strerr even for normal output
//	split_cleanup(argv);
	return (status);//handles blank cmd string...?
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

//contructs args consuming one word at a time, should call multiple times and factor out cmd string
char    **make_argv(t_tok *src, t_env *env)
{
    char		**argv;
	char		*cmd;
	t_shnode	*path;

	argv = expand_all(src, env, collect_argv);//no need double ptr//uhh... she handles glob? half half lah hor//take tok for arg
	if (!argv)
		return (NULL);
	path = find_env("PATH", env->env);
	if (ft_strchr(*argv, '/') || isbuiltin(*argv)
		|| !path || !path->str || !path->str[0])
		return (argv);
	cmd = longest_argv(path->str, *argv);//find the largest env string and use that size
	if (ft_err(-!cmd, "cmd malloc error"))
		return (split_cleanup(argv));
	find_path(path->str, argv, cmd);//free unused string
	return (argv);
}
