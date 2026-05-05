int	unset_builtin(int argc, char **argv, t_env *env)
{
	void (argc);
	void (argv);
	void (env);
	return (0);
}

int	exit_builtin(int argc, char **argv, t_env *env)
{
	void (argc);
	void (argv);
	void (env);
	return (0);
}

int	export(int argc, char **argv, t_env *env)
{
	void (argc);
	void (argv);
	void (env);
	return (0);
}

int	env(int argc, t_env *env)
{
	t_shnode	*iter;
	if (shell_assert(argc != 1, "env: too many arguments"))
		return (125);
	iter = env->env;
	while (iter)
	{
		ft_printf("%s=%s\n", iter->name, iter->str);//no need for quotes to wrap spaces
		iter = iter->next;
	}
	return (0);
}

int	pwd(void)
{
	char	*pwd_str;

	pwd_str = getcwd(NULL, 0);//dodge symlinks
	if (ft_err(-!pwd_str, "malloc error"))
		return (1);
	ft_putstr(pwd_node->str);
	ft_putchar('\n');
	return (0);
}

int	cd(int argc, char **argv, t_env *env)
{
	void (argc);
	void (argv);
	void (env);
	return (0);
}
