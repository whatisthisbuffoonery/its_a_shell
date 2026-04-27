#include "h_minishell.h"

pid_t	subshell_fork(t_env *env)
{
	pid_t	pid;

	pid = fork();
	if (!pid)
		env_import(env);
	return (ft_err(pid, "shell fork"));
}

void	unset(int *fd)
{
	if (*fd > 2)
	{
		close(*fd);
		*fd = -1;
	}
}

void	pipeset_cleanup(t_pipeset *set size_t n)
{
	size_t	i;

	if (!set)
		return ;
	i = 0;
	while (i < n)
	{
		unset(&set[i][0]);
		unset(&set[i][1]);
		i ++;
	}
	free(set);
}

void	pipemanager_init(t_pipemanager *dst, int p_index)
{
	t_pipeset	*set;
	pid_t		*pids;
	int			i;

	i = 0;
	pids = malloc(sizeof(pid_t) * (p_index + 1 + 1));
	if (pids)
		set = ft_calloc(sizeof(t_pipeset), p_index + 1 + 1);
	while (i <= p_index)
	{
		if (ft_err(-(!pids || !set), "pipemanager malloc")
			|| ft_err(pipe(set[i]), "pipe error"))
		{
			pipeset_cleanup(set, i);
			free(pids);
			return ;
		}
		pids[i++] = -1;
	}
	set[i][0] = -1;
	dst->pipes = set;
	dst->pids = pids;
	dst->pipe_count = p_index + 1;
}

void	pid_append(t_pipemanager *p, pid_t src)
{
	if (p->pid_count > p->pipe_count)
	{
		ft_putstr_fd("\n\n============== ya done cooked the process management ===============\n\n");
		exit(1);
	}
	p->pids[p->pid_count] = src;
	p->pid_count += 1;
}

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
int	do_simple(t_node *node, t_env *env, int *redir_fd, int *pfd)
{
	char	**argv;
	char	*cmd;
	int		fd[2];
	int		status;

	argv = NULL;
	if (!node->argv)//redir only path
		return (0);
	set_fd(fd, redir_fd, pfd);//no dup here
	cmd = make_command(node, env);//account for redir as command//expand + glob here
	if (cmd)
		argv = argv_init(cmd, node, env); //free cmd on error internally//expand + glob here
	if (argv)// will create argv even if no args
		status = exec_simple(count_argv(argv), argv, env, fd);
	else
		status = 1;
	simple_nuke(argv, env, redir_fd);
	return (status);
	//maybe dont dup if duped_fd set//VETO : cmd might have redir set
}

//check subshell flag //move all the redir stuff to handle_redir//maybe export redir_fd to simple
int	do_group(t_node *node, t_env *env, int *pfd)
{
	int		status;
	int		redir_fd[2];
	pid_t	pid;

	if(redir_to_fd(node, redir_fd))//open files //expand + glob here
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
		if (pid < 0)
			return (1);
		else if (!pid)
			do_list_and_die(node->left, env, redir_fd, pfd);
		else
			return (child_wait_single(pid));
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
		pid_append(p, pid);
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
		pipemanager_init(p, p_index);
		if (p->pipes)
			do_pipe_command(node->left, env, p, p_index + 1);//make subshell
	}
	if (!p->pipes)
		return (1);
	do_pipe_command(node->right, env, p, p_index);
	if (!p_index)
		return (child_wait_list(p->pids));//do_list parent will cleanup
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

int	pipemanager_append(t_env *env, t_pipemanager **new)
{
	t_pipemanager	*iter;

	*new = ft_calloc(sizeof(t_pipemanager), 1);
	if (!*new)
		return (1);
	iter = env->p;
	while (iter && iter->next)
		iter = iter->next;
	if (iter)
		iter->next = *new;
	else
		env->p = *new;
	return (0);
}
