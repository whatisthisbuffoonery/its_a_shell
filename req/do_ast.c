#include "h_minishell.h"

int	subshell(t_env *env)
{
	pid_t	pid;

	pid = fork();
	if (!pid)
		env_import(env);
	else
		return (child_wait(pid));
	return (0);
}

t_pipeset	*pipeset_init(int p_index)
{
	t_pipeset	*ret;
	int			i;

	i = 0;
	ret = ft_calloc(sizeof(t_pipeset), p_index + 1 + 1);
	while (ret && i <= p_index)
	{
		if (ft_err(pipe(ret[i]), "pipe error"))
			return (pipeset_cleanup(ret, i));
		i ++;
	}
	ret[i][0] = 0;
	ret[i][1] = 1;
	return (ret);
}

int	do_simple(t_node *node, t_env *env, int *fd)
{
	int		argc;
	char	**argv;
	char	**envp;
	char	*cmd;

	cmd = make_command();
}

int	do_group(t_node *node, t_env *env, int *fd)
{
	if (node->kind != N_GROUP)
		return (do_simple(node, env, fd));

}

int	do_pipe_command(t_node *node, t_env *env, t_pipeset *p, int p_index)
{
	int	fd[2];
}

//i.e. a | b | c : c index 0, b index 1 : p malloc 1 + 1 + 1 = 3 :
//a index 2 stdin = p[2][0] = 0, a index 2 - 1 stdout = p[1][1]
//b index 1 stdin = p[1][0], stdout = p[0][1]
//c index 0 stdin = p[0][0], !index: stdout = default to 1/outfile
int	do_pipe(t_node *node, t_env *env, t_pipeset **p, int p_index)
{
	int	fd[2];

	fd[0] = 0;
	fd[1] = 1;
	if (node->kind != N_PIPE)
		return (do_group(node, env, fd));
	else if (node->left->kind == N_PIPE)
		do_pipe(node->left, env, p, p_index + 1);
	else
	{
		*p = pipeset_init(p_index);
		if (!*p)
			return (1);
		do_pipe_command(node->left, env, p, p_index + 1);//make subshell
	}
	do_pipe_command(node->right, env, p, p_index);
	if (!p_index)
		return (child_wait(env->pids));//MAKE SURE THIS DOES NOT GET OVERRIDDEN//suggest grouping pipes and pids
	return (0);
}

//entry point
int	do_list(t_node *node, t_env *env)
{
	int			status;
	t_pipeset	*p;

	p = NULL;
	status = 0;
	if (node->kind != N_AND && node->kind != N_OR)
		return (do_pipe(node, env, &p, 0));
	status = do_list(node->left, env);
	if ((node->kind == N_AND && !status)
		|| (node->kind == N_OR && status))
		status = do_list(node->right, env);
	clean_pipeset(p);
	return (status);
}
