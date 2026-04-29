#include "h_minishell.h"

//recontruct
//will ironically be called from redir_to_fd
char    **make_argv(t_tok *src, t_env *env)
{
    char    **argv`;
	char	*cmd;
    int     start;
    int     i;

	argv = expand_env(src);//no need double ptr//uhh... she handles glob? half half lah hor
	if (!argv)
		return (NULL);
	cmd = *argv;
	if (ft_strchr(cmd, '/'))
		return (argv);
	cmd = longest_path(env, *argv);//find the largest env string and use that size
	if (!cmd)
		return (split_cleanup(argv));
	find_path(env, argv, cmd);//free unused string
	return (ret);
}
