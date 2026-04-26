#include "h_minishell.h"

pid_t	subshell_fork(t_env *env)
{
	pid_t	pid;

	pid = fork();
	if (!pid)
		env_import(env);
	return (pid);
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

int	exec_simple(int argc, char **argv, t_env *env, int *fd)
{
	int		oldfd[2];
	char	**envp;//binary might be a c program that asks for envp

	oldfd[0] = -1;
	oldfd[1] = -1;
	//if builtin with redir, set oldfd and restore before returning
	//if binary, build envp, dup2 fd, then check do_not_subshell
}

//check subshell flag//will wait for its own children
int	do_simple(t_node *node, t_env *env, int *redir_fd, int *pfd)
{
	char	**argv;
//	char	**envp;
	char	*cmd;
	int		fd[2];
	int		status;

	argv = NULL;
	set_fd(fd, redir_fd, pfd);//no dup here
	cmd = make_command(node, env);//account for redir as command//expand + glob here
	if (cmd)
		argv = argv_init(cmd, node, env);//argv pop first ig //free cmd on error internally//expand + glob here
	if (argv)// && !envp_init(&envp, env))
		status = exec_simple(count_argv(argv), argv, env, fd);
	else
		status = 1;
	simple_nuke(argv, env, redir_fd);
	return (status);
	//dup redir if set. if not, dup pfd if set. set env->duped_fd when using dup to close 0 and 1 later
	//maybe dont dup if duped_fd set//VETO : cmd might have redir set
}

//check subshell flag //move all the redir stuff to handle_redir//maybe export redir_fd to simple
int	do_group(t_node *node, t_env *env, int *pfd)
{
	int		status;
	int		redir_fd[2];
	pid_t	pid;

	if(redir_to_fd(node, redir_fd))// //expand + glob here
		return (1);
	if (node->kind != N_GROUP)
		return (do_simple(node, env, redir_fd, pfd));//not do list
	if (env->do_not_subshell)//need to handle redir//just dup2 here, no need to restore
	{
		env->do_not_subshell = 0;//i would check for the left child being another N_GROUP, but eh
		do_list_and_die(node->left, env, redir_fd, pfd);//dup, set duped_fd for cleanup, call do_list, free and close, exit
	}
	else
	{
		pid = shell_fork(env);//clean blanks
		if (!pid)
			do_list_and_die(node->left, env, redir_fd, pfd);
		else
			return (child_wait_single(pid));
	}
	return (0);
}

int	do_pipe_command(t_node *node, t_env *env, t_pipemanager *p, int p_index)
{
	int		status;
	int		fd[2];
	pid_t	pid;

	pid = shell_fork(env);//clean blanks
	if (pid)
	{
		pid_append(&p->pids, pid);
		return (0);
	}
	//child only
	env->do_not_subshell = 1;
	fd[0] = p->pipes[p_index][0];
	fd[1] = 1;
	if (p_index)
		fd[1] = p->pipes[p_index - 1][1];
	status = do_group(node, env, fd);//assumption : child is not a cond
	//clean_pipemanager(p);//child only//env cleanup will do this
	exit(status);
	return (status);
}

//i.e. a | b | c : c index 0, b index 1 : p malloc 1 + 1 + 1 = 3 :
//a index 2 stdin = p[2][0] = 0, a index 2 - 1 stdout = p[1][1]
//b index 1 stdin = p[1][0], stdout = p[0][1]
//c index 0 stdin = p[0][0], !index: stdout = default to 1/outfile
int	do_pipe(t_node *node, t_env *env, t_pipemanager *p, int p_index)
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
		p->pipes = pipeset_init(p_index);
		if (p->pipes)
			do_pipe_command(node->left, env, p, p_index + 1);//make subshell
	}
	if (!p->pipes)
		return (1);
	do_pipe_command(node->right, env, p, p_index);
	if (!p_index)
		return (child_wait_list(p->pids));//rmb to free
	return (0);
}

//entry point
//the alternative to forking is keeping depth counters on blank env vars
int	do_list(t_node *node, t_env *env)
{
	int				status;
	t_pipemanager	*p;

	status = 0;
	if (node->kind != N_AND && node->kind != N_OR)
	{
		if (node->kind == N_PIPE && pipemanager_append(env, &p))//I hate myself//append new item and set p to that item
			return (1);
		status = do_pipe(node, env, &p, 0);
		if (node->kind == N_PIPE)
			pipemanager_pop(env);
		return (status);
	}
	status = do_list(node->left, env);
	if ((node->kind == N_AND && !status)
		|| (node->kind == N_OR && status))
		status = do_list(node->right, env);
	return (status);
}
