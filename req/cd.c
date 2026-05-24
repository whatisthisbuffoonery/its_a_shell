#include "h_minishell.h"

int	update_pwd(t_env *env, char *v)
{}

void	modify_relative(char *v, int *index)
{
	int	i;

	i = 0;
	while (v[i + *index] && v[i + *index] != '/')
		i ++;
	if (v[*index] == '.' && i > 1)// ../
		path_decrement_double(v, &i, index);//no op if v has no other leading elements besides ./ and ../ //self increment
	else if (v[*index] == '.')
	{
		i = *index;
		while (1)
		{
			v[i] = v[i + 1 + (v[i + 1] != '\0')];
			if (!v[i])
				break ;
		}
	}
	else
		*index += i + (v[*index] == '/' && v[*index + 1]);
}

//I still havent init pwd
//psa $PWD restores from internal value, even if unset
int	new_pwd(char **dst, char **pwd, char **v)
{
	char		*new_pwd;
	int			i;

	if (v[1][0] == '/')
	{
		*dst = v[1];
		i = 1;
		while (v[i])
		{
			v[i] = v[i + 1];
			i ++;
		}
		return (0);
	}
	else if (!*pwd)
	{
		*pwd = getcwd(NULL, 0);
		if (ft_err(-!*pwd, "internal pwd replacement malloc"))
			return (1);
	}
	else if (path_syntax_check(v[1]))
		return (1);
	i = 0;
	while (v[1][i] && v[1][i] != '/')
		resolve_relative(*pwd, v[1], &i);
}

//resolve first.
//parse syntax first. //cant use stat
int	cd(int argc, char **argv, t_env *env)
{
	char	*chdir_dst;

	if (shell_assert(argc > 2, "too many arguments"))
		return (1);
	else if (!argv[1] || !argv[1][0])
		return (0);
	else if (new_pwd(&chdir_dst, &env->pwd, argv) || ft_err(chdir(argv[1]), "cd"))
		return (1);
	return (update_pwd(env, argv[1]));
}
