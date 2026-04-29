#include "h_minishell.h"

void	unset(int *fd)
{
	if (*fd > 2)
	{
		close(*fd);
		*fd = -1;
	}
}

void	set_fd(int *fd, int *pfd)
{
	if (fd[0] < 2)
		fd[0] = pfd[0];
	else
		unset(&pfd[0]);
	if (fd[1] < 2)
		fd[1] = pfd[1];
	else
		unset(&pfd[1]);
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

int	pipe_dup(int *fd)
{
	fd[0] = ft_err(dup(fd[0]), "dup error");
	if (fd[0] < 0)
		return (1);
	fd[1] = ft_err(dup(fd[1]), "dup error");
	if (fd[1] < 0)
	{
		unset(&fd[0]);
		return (1);
	}
	return (0);
}
