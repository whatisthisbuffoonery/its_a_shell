#include "h_minishell.h"

int	exec_simple(int argc, char **argv, t_env *env, int *fd)
{
	int		oldfd[2];
	char	**envp;//binary might be a c program that asks for envp

	oldfd[0] = -1;
	oldfd[1] = -1;
	//if builtin with redir, set oldfd and restore before returning
	//if binary, check do_not_subshell, build envp, dup2 fd
}

//check subshell flag//will wait for its own children
int	do_simple(t_node *node, t_env *env, int *fd)
{
	char	**argv;
	char	*cmd;
	int		status;

	argv = NULL;
	if (!node->argv)//redir only path
		return (0);
	cmd = make_command(node, env);//account for redir as command//expand + glob here
	if (cmd)
		argv = argv_init(cmd, node, env); //free cmd on error internally//expand + glob here
	if (argv)// will create argv even if no args
		status = exec_simple(count_argv(argv), argv, env, fd);
	else
		status = 1;
	split_cleanup(argv);
	unset(&fd[0]);//pass cleanup to exec_simple
	unset(&fd[1]);
	return (status);
}

int	do_list_subshell(t_node *node, t_env *env, int *fd)
{
	int	status;
	int	flag;

	status = 1;
	flag = 0;
	if (!ast_dup(env, fd))
	{
		if (fd[0] > 2)
			env->duped_fd[0] = 1;
		if (fd[1] > 2)
			env->duped_fd[1] = 1;
		status = do_list(node, env);
	}
	shell_cleanup(env, fd);
	return (status);
}

//check subshell flag //move all the redir stuff to handle_redir//maybe export redir_fd to simple
int	do_group(t_node *node, t_env *env, int *pfd)
{
	int		status;
	int		fd[2];
	pid_t	pid;

	if(redir_to_fd(node, fd) || set_fd(fd, pfd))//init fd to 0, 1 //open files //expand + glob here
		return (1);
	if (node->kind != N_GROUP)
		return (do_simple(node, env, fd));//not do list
	if (env->do_not_subshell)//need to handle redir//just dup2 here, no need to restore
	{
		env->do_not_subshell = 0;//i would check for the left child being another N_GROUP, but eh
		return (do_list_subshell(node->left, env, fd));//dup, set duped_fd for cleanup, call do_list, free and close, exit
	}
	else
	{
		pid = shell_fork(env);//clean blanks
		if (pid < 0)
			return (1);
		else if (!pid)
			exit(do_list_subshell(node->left, env, fd));
		else
			return (child_wait(pid));
	}
	return (0);
}

int	do_pipe_command(t_node *node, t_env *env, t_pipemanager *p, int p_index)
{
	int			status;
	int			fd[2];
	pid_t		pid;

	pid = shell_fork(env);//clean blanks
	if (pid < 0)
		return (1);
	else if (pid)
	{
		pid_bump(p, pid);
		return (0);
	}
	//child only
	env->do_not_subshell = 1;
	fd[0] = p->pipes[p_index][0];
	if (fd[0] < 0)
		fd[0] = 0;
	fd[1] = 1;
	if (p_index)
		fd[1] = p->pipes[p_index - 1][1];
	status = do_group(node, env, fd);//assumption : child is not a cond //child needs cleanup
	exit(status);
	return (status);
}

//i.e. a | b | c : c index 0, b index 1, a index 2 : p malloc 2 + 1 = 3 :
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
		pipemanager_init(p, p_index);
		if (p->pipes)
			do_pipe_command(node->left, env, p, p_index + 1);//make subshell
	}
	if (!p->pipes)
		return (1);
	do_pipe_command(node->right, env, p, p_index);
	if (!p_index)
		return (child_wait(p->pid));//do_list parent will cleanup //waitpid for the pid, then wait until it returns -1 and not for sigint
	return (0);
}

//entry point
//the alternative to forking is keeping depth counters on blank env vars
//and redir/pipe fds. hell no.
int	do_list(t_node *node, t_env *env)
{
	int				status;
	t_pipemanager	*p;

	status = 0;
	if (node->kind != N_AND && node->kind != N_OR)
	{
		if (node->kind == N_PIPE && pipemanager_append(env, &p))//I hate myself//append new item and set p to that item
			return (1);
		status = do_pipe(node, env, p, 0);
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
