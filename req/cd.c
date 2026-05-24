#include "h_minishell.h"

int	update_pwd(t_env *env, char *v)
{}

void	path_process(char *v)
{
	int	i;
	int	k;
	int	tmp;

	i = 0;
	while(v[i])
	{
		k = 0;
		while (v[i] && v[i] != '/')
			i ++;
		while (v[i + k] && v[i + k] == '/')
			k ++;
		k -= (v[i] == '/');
		tmp = i + 1;
		while (k)
		{
			v[tmp] = v[tmp + k];
			if (!v[tmp])
				break ;
			tmp ++;
		}
		i += (v[i] != '\0');
	}
//	if (i > 1 && v[i - 1] == '/')
//		v[i - 1] = '\0';
	//VETO
}

void	resolve_relative(char *pwd, char *v, int *index)
{
	int	i;

	i = 0;
	while (v[i + *index] && v[i + *index] != '/')
		i ++;
	if (v[*index] == '.' && v[*index + 1] == '.' && i == 2)// ../ // ... is a different dirname
		path_decrement(pwd, v, i, index);//no op if v and pwd both have no other leading elements besides ./ and ../ //self increment
										 //cd .. from /root resolves to /root, delete .. and skip decrement in this case
										 //functions by shoving the string to the left and setting index
	else if (v[*index] == '.' && i == 1)
	{
		i = *index;
		while (v[i])
		{
			v[i] = v[i + 1 + (v[i + 1] != '\0')];
			if (v[i])
				i ++;
		}
	}
	else
		*index += i + (v[*index + i] == '/');// && v[*index + 1]);
}

//I still havent init pwd
//psa $PWD restores from internal value, even if unset
//if v ends up empty, modify free flag
int	new_pwd(char **dst, char **pwd, char **v, int *flag)
{
	char		*new_pwd;
	int			i;

	path_process(v[1]);//opportunity to print result and no op
	if (v[1][0] == '/')
	{
		*dst = v[1];
		v[1] = NULL;
		return (0);
	}
	else if (!*pwd)
	{
		*pwd = getcwd(NULL, 0);
		if (ft_err(-!*pwd, "internal pwd replacement malloc"))
			return (1);
	}
	i = 0;
	while (v[1][i] && v[1][i] != '/')
		resolve_relative(*pwd, v[1], &i);//here too
	//actually assign to dst
}

//resolve first.
int	cd(int argc, char **argv, t_env *env)
{
	int		should_free_v;
	char	*chdir_dst;

	should_free_v = 1;
	if (shell_assert(argc > 2, "too many arguments"))
		return (1);
	else if (!argv[1])
	{
		should_free_v = 0;
		argv[1] = grab_home(env);//dont return null ples
	}
	if (!argv[1][0])
	{
		if (!should_free_v)
			argv[1] = NULL;
		return (0);
	}
	else if (element_check(argv[1]))//stat each item
		return (1);
	if (new_pwd(&chdir_dst, &env->pwd, argv) || ft_err(chdir(chdir_dst), "cd"))
		return (1);
	return (update_pwd(env, chdir_dst, should_free_v));//if chdir[0] == '/' && !should, else if !should, ...
}
