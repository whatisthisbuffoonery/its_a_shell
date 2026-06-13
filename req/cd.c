#include "h_minishell.h"

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
}

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
	index -= (index > 0 && str[index - 1] == '/') + (index > 1);
	while (index > 0 && str[index - 1] != '/')
		index --;
	if (src)
		*src = index;
	ft_printf("cd component: %s, index: %d\n", str, index);
}
/*
int	check_stat(char *pwd, int *src, char *err)
{
	int			curr;
	int			ret;
	char		tmp;
	struct stat	dump;

	curr = *src;
	int debug = curr;
	
	if (curr > 1 && pwd[curr - 1] == '/')
		curr --;
	tmp = pwd[curr + 1];
	ft_printf("stat debug: curr: %d, curr + 1: %d, strlen: %d, char: %c:%d, str: %s", debug, curr, ft_strlen(pwd), tmp, tmp, pwd);
	exit(0);
	pwd[curr + 1] = '\0';
	ret = stat(pwd, &dump);
	ft_printf("stat: vlen: buf: %s, result: %d\n", pwd, ret);
	pwd[curr + 1] = tmp;
	ft_printf("pwd reset check: %s\n", pwd);
	(void) dump;
	return (builtin_err(ret, "cd", err));
}
*/
//cd .. from /root resolves to /root, delete .. and skip decrement in this case
//functions by shoving the string to the left and setting index
int	resolve_relative(char *pwd, int *src)
{
	int	i;
	int index;

	i = 0;
	index = *src;
	if (!pwd[index] || (pwd[index] == '/' && !pwd[index + 1]))
		return (1);
	while (pwd[i + index] && pwd[i + index] != '/')
		i ++;
	ft_printf("resolve: index: %d, i: %d, str: %s\n", index, i, pwd);
	if (pwd[index] == '.' && pwd[index + 1] == '.' && i == 2)// ../ // ... is a different dirname
	{
		remove_component(pwd, src);//delete ../ first (, new src -= 1 + (src > 1 + 1)
		if (index > 1)
			remove_component(pwd, src);
	}
	else if (pwd[index] == '.' && i == 1)
		path_strmove(&pwd[index], i + 1);
	else
	{
		*src += i + (pwd[index + i] == '/');
	//	return (check_stat(pwd, src, err));
	}
	ft_printf("cd intermediate: %s\n", pwd);
	return (0);
}
/* VETO
int	check_pwd(char **dst, char **src)//suggest using newpwd buffer to handle instead
{
	*dst = NULL;
	if (!*src)
		*src = getcwd(NULL, 0);
	if (*src)
		*dst = ft_strdup(*src);
	return (ft_err(-!*dst, "cd pwd prep"));
}*/

//I still havent init pwd
//psa $PWD restores from internal value, even if unset
//if v ends up empty, modify free flag
int	new_pwd(char *pwd, char *v)
{
	int			i;
	int			done;
	struct stat	dump;

	if (v[0] == '/')
	{
		ft_strlcpy(pwd, v, -1);
		return (builtin_err(-1 * stat(pwd, &dump), "cd abs", v));
	}
	path_process(pwd);
	i = ft_strlen(pwd);
	if (pwd[i - 1] != '/')
	{
		i ++;
		ft_strlcat(pwd, "/", -1);
	}
	ft_strlcat(pwd, v, -1);
	path_process(pwd);
	done = builtin_err(stat(pwd, &dump), "cd rela", v);
	while (!done)
		done = resolve_relative(pwd, &i);//here too
	ft_printf("cd result: done:%d, done bool:%d, %s, %s\n", done, (done < 0), pwd, v);
	(void) dump;
	return (done < 0);
}
/*
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
}*/
