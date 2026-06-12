#include "h_minishell.h"

int	update_pwd(t_env *env, char *v)
{
	(void) env;
	(void) v;
	return (0);
}

void	path_strmove(char *dst, int n)
{
	int		i;
	char	*src;

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
		path_strmove(&v[i], k);
		i += (v[i] != '\0');
	}
//	ft_putstr(v);
//	ft_putchar('\n');
//	exit(0);
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
	ft_printf("remove: index: %d, i: %d, str: %s\n", index, i, str);
	
	i += (str[index + i] == '/');
	path_strmove(&str[index], i);
//	exit(0);
	index -= (index > 0 && str[index - 1] == '/') + (index > 1);
	while (index > 0 && str[index - 1] != '/')
		index --;
	if (src)
		*src = index;
	ft_printf("cd component: %s, index: %d\n", str, index);
//	exit(0);
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
	ft_printf("resolve: index: %d, i: %d, str: %s\n", index, i, pwd);
//	exit(0);
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
	ft_printf("cd intermediate: %s, %s\n", pwd, v);
	return (check_stat(pwd, v, src));
}

int	check_pwd(char **dst, char **src)//suggest using newpwd buffer to handle instead
{
	*dst = NULL;
	if (!*src)
		*src = getcwd(NULL, 0);
	if (*src)
		*dst = ft_strdup(*src);
	return (ft_err(-!*dst, "cd pwd prep"));
}

//I still havent init pwd
//psa $PWD restores from internal value, even if unset
//if v ends up empty, modify free flag
int	new_pwd(char **pwd, char **v, int *status)
{
	int		i;
	int		done;

	path_process(v);//opportunity to print result and no op
	if (v[0] == '/')
	{
		free(*pwd);
		*pwd = *v;
		*v = NULL;
		return (0);
	}
	i = 0;
	done = 0;
	while (!done)//maybe
		done = resolve_relative(pwd, v, &i);//here too
//	*dst = join_path(*pwd, v[1]);
	ft_printf("cd result: %s, %s\n", *pwd, v);
//	exit(0);
	return (done < 0);
}

int element_check(char *v)
{
	(void) v;
	return (0);
}

//resolve first.
int	cd_trial(int argc, char **argv, t_env *env)
{
	int		should_free_v;
	char	*chdir_dst;

	should_free_v = 1;
	chdir_dst = NULL;
	if (shell_assert(argc > 2, "too many arguments"))
		return (1);
	else if (!argv[1])
	{
		return (0);
	//	should_free_v = 0;
	//	argv[1] = grab_home(env);//dont return null ples //emit error
	}
	ft_printf("cd start: %s\n", argv[1]);
	if (!argv[1][0])
	{
		ft_putstr("huhhh\n");
		if (!should_free_v)
			argv[1] = NULL;
		return (1);
	}
//	else if (element_check(argv[1]))//stat each item
//		return (1);
	//if (new_pwd(&chdir_dst, &env->pwd, argv) || ft_err(chdir(chdir_dst), "cd"))
	//	return (1);
	new_pwd(&chdir_dst, &env->pwd, argv);
//	ft_putstr(chr_dst);
//	ft_putchar('\n');
	free(chdir_dst);
//	return (update_pwd(env, chdir_dst, should_free_v));//if chdir[0] == '/' && !should, else if !should, ...
	return (0);
}
