#include "h_minishell.h"

void	unset(int *fd)
{
	if (*fd > 2)
	{
		close(*fd);
		*fd = -1;
	}
}

int	set_fd(int *fd, int *pfd)
{
	if (fd[0] <= 2)
		fd[0] = ft_err(dup(pfd[0]), "dup error");
	if (fd[1] <= 2)
		fd[1] = ft_err(dup(pfd[1]), "dup error");
	if (fd[0] < 0 || fd[1] < 0)
	{
		unset(&fd[0]);
		unset(&fd[1]);
		return (1);
	}
	return (0);
}

void	pipeset_cleanup(t_pipeset *set, size_t n)
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
	int			i;

	i = 0;
	set = malloc((p_index + 1) * sizeof(t_pipeset));
	while (i < p_index)
	{
		if (ft_err(-!set, "pipemanager malloc")
			|| ft_err(pipe(set[i]), "pipe error"))
		{
			pipeset_cleanup(set, i);
			return ;
		}
		i ++;
	}
	set[i][0] = -1;
	set[i][1] = -1;
	dst->pipes = set;
	dst->pid = 0;
	dst->pid_count = 0;
	dst->pipe_count = p_index;
}

int	pipemanager_append(t_env *env, t_pipemanager **new)
{
	t_pipemanager	*iter;

	*new = ft_calloc(1, sizeof(t_pipemanager));
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
