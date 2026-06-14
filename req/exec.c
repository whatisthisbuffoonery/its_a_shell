/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:29 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:29 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	do_binary(char **argv, t_env *env, int *fd)
{
	char	**envp;
	int		status;

	status = signo;
	envp = NULL;
	errno = 0;
	if (!status)
		envp = make_envp(env, &status);//!envp is not the fail condition
	if (!status)
		status = ft_err(-(dup2(fd[0], 0) < 0 || dup2(fd[1], 1) < 0), "dup error binary");//valgrind you jerk
	unset(&fd[0]);
	unset(&fd[1]);
	if (!status)
		ft_err(execve(*argv, argv, envp), *argv);
	status = 1;
	if (errno == ENOENT)
		status = 127;
	else if (errno == EACCES)
		status = 126;
	shell_cleanup(env);
	split_cleanup(argv);
	split_cleanup(envp);
	return (status);
}
/*
//where tf are the dups for this one
int	do_builtin(int argc, char **argv, t_env *env, int *fd)
{
	int		oldfd[2];
	int		status;

	oldfd[0] = ft_err(dup(0), "dup error");
	oldfd[1] = ft_err(dup(1), "dup error");
	status = 1;
	if (!signo && oldfd[0] > 0 && oldfd[1] > 0
		&& !ft_err(dup2(fd[0], 0), "dup error"))
	{
		if (ft_err(dup2(fd[1], 1), "dup error"))
			ft_err(dup2(oldfd[0], 0), "restore error");
		else
		{
			status = do_builtin_match(argc, argv, env);//do the actual builtin
			ft_err(dup2(oldfd[0], 0), "restore error");
			ft_err(dup2(oldfd[1], 1), "restore error");
		}
	}
	split_cleanup(argv);
	unset(&oldfd[0]);
	unset(&oldfd[1]);
	unset(&fd[0]);
	unset(&fd[1]);
	return (status);
}
*/

//exit builtin was leaking fd and oldfd to begin with. do not set duped fd here, clean in exit.

int	exec_simple(int argc, char **argv, t_env *env, int *fd)
{
	pid_t	pid;

	if (isbuiltin(*argv))
		return (do_builtin_match(argc, argv, env, fd));
	pid = 0;
	if (!env->do_not_subshell)
		pid = shell_fork(env);
	if (pid < 0)
		return (1);
	env->do_not_subshell = 0;//slightly useless now that I look at it
	if (!pid)
		exit(do_binary(argv, env, fd));//also cleans env
	else
		return (child_wait(pid));
}

int	do_redir(int *fd)
{
	unset(&fd[0]);
	unset(&fd[1]);
	return (0);
}

//check subshell flag//will wait for its own children
int	do_simple(t_node *node, t_env *env, int *fd)
{
	char	**argv;
	int		status;
	int		i;

	argv = NULL;
	if (!node->argv)//redir only path
		return (do_redir(fd));
	argv = make_argv(node->argv, env); //free cmd on error internally//expand + glob here
	i = 0;
	while (argv && argv[i])
		i ++;
	if (argv)// will create argv even if no args
		status = exec_simple(i, argv, env, fd);
	else
		status = 1;
	split_cleanup(argv);
	unset(&fd[0]);
	unset(&fd[1]);
	return (status);
}

int	do_list_subshell(t_node *node, t_env *env, int *fd)
{
	int	status;
	int	flag[2];

	status = 1;
//	flag[0] = 0;
//	if (fd[0] > 2)
	flag[0] = ft_err(dup2(fd[0], 0), "dup error subshell");
//	flag[1] = 1;
//	if (fd[1] > 2)
	flag[1] = ft_err(dup2(fd[1], 1), "dup error subshell");//bruh
	if (fd[0] > 2 && flag[0] >= 0)
		env->duped_fd[0] = 1;
	if (fd[1] > 2 && flag[1] >= 0)
		env->duped_fd[1] = 1;
	unset(&fd[0]);
	unset(&fd[1]);
	env->is_in_subshell = 1;
	if (flag[0] >= 0 && flag[1] >= 0)
		status = do_list(node, env);
	shell_cleanup(env);//needed, do_list, do_pipe and do_group never clean up
	return (status);
}

//check subshell flag //move all the redir stuff to handle_redir//maybe export redir_fd to simple
// do_list_subshell cleans out env, do_simple from the pipe child path executing a builtin wont.
//define double env cleanup by setting elements to null
int	do_group(t_node *node, t_env *env, int *pfd)
{
	int		fd[2];
	pid_t	pid;

	if (signo)//could race if combined with redir cond
		return (signo);
	else if (redir_to_fd(node, env, fd, pfd))//init fd to 0, 1 //open files //expand + glob here //call set_fd
		return (signo + !signo);
	else if (node->kind != N_GROUP)
		return (do_simple(node, env, fd));//not do list
	else if (env->do_not_subshell)//need to handle redir//just dup2 here, no need to restore
	{
		env->do_not_subshell = 0;//i would check for the left child being another N_GROUP, but eh
		return (do_list_subshell(node->left, env, fd));//dup, set duped_fd for cleanup, call do_list, free and close, exit
	}
	else
	{
		pid = shell_fork(env);//clean blanks
		if (!pid)
			exit(do_list_subshell(node->left, env, fd));
		unset(&fd[0]);
		unset(&fd[1]);
		return (child_wait(pid));
	}
}

void	do_pipe_command(t_node *node, t_env *env, t_pipemanager *p, int p_index)
{
	int			status;
	int			fd[2];
	pid_t		pid;

	pid = shell_fork(env);//clean blanks
	pid_bump(p, pid);//take negative pid wait as failure return status
	if (pid)
		return ;
	env->do_not_subshell = 1;//child only route
	fd[0] = p->pipes[p_index][0];
	p->pipes[p_index][0] = -1;
	if (fd[0] < 0)
		fd[0] = 0;
	fd[1] = 1;
	if (p_index)
	{
		fd[1] = p->pipes[p_index - 1][1];
		p->pipes[p_index - 1][1] = -1;
	}
	// env->duped_fd[0] += (fd[0] > 2);
	// env->duped_fd[1] += (fd[1] > 2);
//	status = pipe_dup(fd);
	clean_pipemanager(p);
	status = do_group(node, env, fd);//assumption : child is not a cond //child needs cleanup
	shell_cleanup(env);//child->builtin path
	exit(status);
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
	if (node->kind != N_PIPE || signo)
		return (do_group(node, env, fd));
	else if (node->left->kind == N_PIPE)
		do_pipe(node->left, env, p, p_index + 1);
	else
	{
		ft_putstr_fd("\n\n======= pipe init: ", 2);
		ft_putnbr_fd(p_index + 1, 2);
		ft_putstr_fd(" ======\n\n", 2);
		pipemanager_init(p, p_index + 1);
		if (p->pipes)
			do_pipe_command(node->left, env, p, p_index + 1);//make subshell
	}
	if (!p->pipes)
		return (1);
	do_pipe_command(node->right, env, p, p_index);
	if (!p_index)
	{
		clean_pipemanager(p);
		return (child_wait(p->pid));
	}
	return (0);
}

void	update_last(t_env *env, int n)
{
	int	i;
	int	t;

	if (n > 255 || n < 0)
		n = 1;
	if (env->last != n)
	{
		i = 0;
		t = 1;
		while (n / t > 9)
			t *= 10;
		while (t)
		{
			env->last_string[i++] = ((n / t) % 10) + '0';
			t /= 10;
		}
		env->last_string[i] = '\0';
		env->last = n;
	}
}

//entry point
//the alternative to forking is keeping depth counters on blank env vars
//and redir/pipe fds. hell no.


//plsss check for signals before entering
int	do_list(t_node *node, t_env *env)	//remember to update last from outside entry
{
	int				status;
	t_pipemanager	p;

	p.pipes = NULL;
	p.pipe_count = 0;
	p.pid_count = 0;
	p.pid = 0;
	if (node->kind != N_AND && node->kind != N_OR)
		return (do_pipe(node, env, &p, 0));
	status = do_list(node->left, env);
	if (signo == SIGINT || (env->is_in_subshell && signo))
		return (signo);
	signo = 0;	//should be sufficient. group and pipe will also check signo.
	update_last(env, status);//propose only having this here
	if (!signo && ((node->kind == N_AND && !status)
		|| (node->kind == N_OR && status)))
		status = do_list(node->right, env);
	return (status);
}
