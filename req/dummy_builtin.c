#include "h_minishell.h"

int	cd(int argc, char **argv, t_env *env)
{
	(void) argc;
	(void) argv;
	(void) env;
	return (0);
}
int	pwd(t_env *env, int out)
{
	(void) env;
	(void) out;
	return (0);
}
int	env_builtin(int argc, t_env *env, int out)
{
	(void) argc;
	(void) out;
	(void) env;
	return (0);
}
int	export(int argc, char **argv, t_env *env, int out)
{
	(void) argc;
	(void) argv;
	(void) env;
	(void) out;
	return (0);
}
int	unset_builtin(int argc, char **argv, t_env *env)
{
	(void) argc;
	(void) argv;
	(void) env;
	return (0);
}
