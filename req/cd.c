#include "h_minishell.h"

int	update_pwd(t_env *env, char *v)
{}

void	path_strmove(char *dst, int n)
{
	int		i;
	char	src;

	if (!dst || n <= 0)//assumes dst is always to the left of src
		return ;
	i = 0;
	src = &dst[n];
	while (1)
	{
		dst[i] = src[i];
		if (!dst[i])
			break ;
		i ++;
	}
}

void	path_process(char *v)
{
	int	i;
	int	k;

	i = 0;
	while(v[i])
	{
		k = 0;
		while (v[i] && v[i] != '/')
			i ++;
		i += (v[i] == '/');
		while (v[i + k] && v[i + k] == '/')
			k ++;
		path_strmove(&v[i + (v[i] == '/')], k);
		i += (v[i] != '\0');
	}
}

//a. check that we even have enough buffer to fill the gap with
//b. that last if is for setting src to the previous item
//c. needs an int to recall the index of the last successful cd incase of cwd disappearing
//d. check for trailing '/'
void	remove_component(char *str, int *src)
{
	int	i;
	int	index;

	i = 0;
	if (src)
		index = *src;
	else
	{
		index = ft_strlen(str) - 1;
		while (index > 0 && str[index - 1] != '/')
			index --;
	}
	while (str[index + i] && str[index + i] != '/')
		i ++;
	if (!i)
		return ;
	i += (str[index + i] == '/');
	path_strmove(&str[index], i);
	index -= (index > 0 && str[index - 1] == '/') + (index > 1);
	while (index > 0 && str[index - 1] != '/')
		index --;
	if (src)
		*src = index;
}

//cd .. from /root resolves to /root, delete .. and skip decrement in this case
//functions by shoving the string to the left and setting index
int	resolve_relative(char *pwd, char *v, int *src)
{
	int	i;
	int index;

	i = 0;
	index = *src;
	if (!v[index] || (v[index] == '/' && !v[index + 1]))
		return (1);
	while (v[i + index] && v[i + index] != '/')
		i ++;
	if (v[index] == '.' && v[index + 1] == '.' && i == 2)// ../ // ... is a different dirname
	{
		remove_component(v, src);//delete ../ first (, new src -= 1 + (src > 1 + 1)
		if (index > 1)
			remove_component(v, src);
		else
			remove_component(pwd, NULL);
	}
	else if (v[index] == '.' && i == 1)
		path_strmove(&v[index], i + 1);
	else
		*src += i + (v[index + i] == '/');// && v[*index + 1]);
	return (0);
}

//I still havent init pwd
//psa $PWD restores from internal value, even if unset
//if v ends up empty, modify free flag
int	new_pwd(char **dst, char **pwd, char **v, int *flag)
{
	char	*new_pwd;
	int		i;
	int		done;

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
	done = 0;
	while (!done)//maybe
		done = resolve_relative(*pwd, v[1], &i);//here too
	*dst = join_path(*pwd, v[1]);
	return (!*dst);
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
		argv[1] = grab_home(env);//dont return null ples //emit error
	}
	if (!argv[1][0])
	{
		if (!should_free_v)
			argv[1] = NULL;
		return (1);
	}
	else if (element_check(argv[1]))//stat each item
		return (1);
	if (new_pwd(&chdir_dst, &env->pwd, argv) || ft_err(chdir(chdir_dst), "cd"))
		return (1);
	return (update_pwd(env, chdir_dst, should_free_v));//if chdir[0] == '/' && !should, else if !should, ...
}
