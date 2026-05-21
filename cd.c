#include "h_minishell.h"

int	update_pwd(t_env *env, char *v)

int	new_pwd(char **dst, char *v, t_env *env)
{
	char	*new_pwd;
	k
	int		i;

	if (!v)
		return (0);
	i = 0;
	//account for missing pwd by replacing here //additional strcmp needed for no op
	len = 
	while (v[i])
	{
		if (v[i] == '.' && v[i + 1] == '.')
			len = 
	}
	ret = 0;
	}
}
int	cd(int argc, char **argv, t_env *env)
{
	char	*chdir_dst;

	if (shell_assert(argc > 2, "too many arguments"))
		return (1);
	else if (!argv[1] || empty_string(argv[1]))//check for whitespace only
		return (0);
	else if (new_pwd(&chdir_dst, v[1]))
		return (1);
	else if (argv[1] && ft_err(chdir(argv[1]), "chdir"))
		return (1);
	return (update_pwd(env, argv[1]));
}
