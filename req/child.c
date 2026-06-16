/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:11 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:11 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

void	env_import(t_env *env)
{
	t_shnode	*iter;
	t_shnode	*prev;

	prev = NULL;
	iter = env->export;
	while (iter)
	{
		if (!iter->str)
		{
			if (prev)
				prev->next = iter->next;
			else
				env->export = iter->next;
			clean_shnode(&iter);
			if (prev)
				iter = prev->next;
			else
				iter = env->export;
			continue ;
		}
		prev = iter;
		iter = iter->next;		
	}
	update_shell_lvl(env, 1);
}

pid_t	shell_fork(t_env *env)
{
	pid_t	pid;

	(void)env;
	if (signo)
		return (-1);
	pid = fork();
	if (!pid)
		env_import(env); //why is this function needed?
	return (ft_err(pid, "shell fork"));
}

void	pid_bump(t_pipemanager *p, pid_t src)
{
	if (p->pid_count > p->pipe_count)
	{
		ft_putstr_fd("\n\n============== ya done cooked the process management ===============\n\n", 2);
		exit(1);
	}
	p->pid = src;
	p->pid_count += 1;
}

//check for negative pid and set status to 1
int	child_wait(pid_t pid)
{
	int	status;
	int	n;

	errno = 0;
	status = 1;
	while (pid > 0 && waitpid(pid, &n, 0) < 0 && errno == EINTR)
		errno = 0;
	if (pid < 1)
		status = signo + !signo;
	else if (errno != ECHILD)
	{
		status = WEXITSTATUS(n);
		if (WIFSIGNALED(n))
			status = WTERMSIG(n) + 128;
		errno = 0;
	}
	while (1)
    {
        if (wait(NULL) < 0 && errno != EINTR)
            break ;
        errno = 0;
    }
	return (status);
}
