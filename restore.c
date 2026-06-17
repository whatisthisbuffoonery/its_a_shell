#include "h_minishell.h"

int	restore_userhome(t_env *env)
{
	char	*user;
	char	*home;
	char	*buf;
	int		passwd;

	user = restore_user(env);
	if (!user)
		return (1);
	passwd = open("/etc/passwd", O_RDONLY);
