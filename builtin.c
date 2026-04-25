int find_command()

if (node->kind == N_CMD && "match builtin cmd name")
	builtin(...);


int	echo(char **v)
{
	int	flag;
	int	i;

	flag = (v[1] && !ft_strcmp(v[1], "-n"));
	i = 1 + flag;
	while (v[i])
	{
		ft_putstr(v[i]);
		i ++;
	}
	if (!flag)
		ft_putchar('\n');
	return (0);
}

int	pwd(void)
{
	//thing, check for call failure
}

int	env(/*I need env*/)
{
	//
}

int	cd(char **v){}

int	export(char **v){}//done...?

int	exit(char **v)
{
	//check arg validity
}

int	unset(char **v, /*env*/) {}

int	no_command(void)
{
	return (0);
}
