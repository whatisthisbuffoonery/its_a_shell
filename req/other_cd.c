#include "h_minishell.h"

char	*grab_v(char **v, t_env *env, int *status)
{
	char	*ret;

	ret = v[1];
	if (!ret || !ft_strcmp(ret, "~"))
		ret = grab_home(env);
	else
		v[1] = NULL;
	*status = shell_assert2(!ret, "cd", "could not resolve HOME");//make builtin version that uses errno
	return (ret);
}

int	cd(int argc, char **argv, t_env *env)
{
	char	*v;
	char	*dst;
	int		status;

	if (shell_assert(argc > 2, "too many arguments"))
		return (1);
	v = grab_v(argv, env, &status);//never free
	if (!v || !v[0])
		return (status);
	if (!env->pwd)
		env->pwd = getcwd(NULL, 0);
	dst = NULL;
	if (env->pwd)
		dst = malloc(ft_strlen(v) + ft_strlen(env->pwd) + 1);
	status = shell_assert2(!dst, "cd", "malloc error");
	if (dst)//need to check pwd null
	{
		ft_strlcpy(dst, env->pwd, -1);
		new_pwd(&dst, &v, &status);
	}
	free(v);
	free(dst);
	return (status);
}