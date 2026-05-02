#include "h_minishell.h"

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

int	select_fd(int *fd, int *pfd, int flag)
{
	if (flag)
	{
		unset(&fd[0]);
		unset(&fd[1]);
		unset(&pfd[0]);
		unset(&pfd[1]);
	}
	else
		set_fd(fd, pfd);
	return (flag);
	//if flag, close all fds
	//else, set_fd
}

int	pipex_cmp(char *line, char *v, int v_len)
{
	int	line_len;

	line_len = ft_strlen(line);
	if (line_len != v_len + 1 || line[v_len] != '\n')
		return (1);
	return (ft_strncmp(line, v, v_len));
}

void	heredoc_write(int fd, char *s)
{
	int	i;
	int	len;

	len = ft_strlen(s);
	i = 0;
	while (i < len && !muh_number && !errno)
		i += write(fd, s, len - i);
	free(s);
}

int	do_heredoc(char *file)
{
	int		fd[2];
	char	*buf;
	int		v_len;

	errno = 0;
	buf = NULL;
	v_len = ft_strlen(file);
	if (ft_err(pipe(fd), "pipe error"))
		return (-1);
	while (!muh_number)//need to test
	{
		buf = gnl_b(0);
		if(!pipex_cmp(buf, file, v_len))
			break ;
		heredoc_write(fd[1], buf);
	}
	close(fd[1]);
	if (muh_number)
		unset(&fd[0]);
	return (fd[0]);//i guess...?
}

//no need to iterate here
int	update_redir_fd(int *fd, char **file, char *op)
{
	int	flag;
	int	id;
	int	fd;

	flag = 0;
	if (op[0] == '<')
	{
		if (op[1])
			fd = do_heredoc(*file);
		else
			fd = ft_err(open(*file, O_RDONLY), "open error");
		id = 0;
	}
	else if (op[0] == '>')
	{
		if (op[1])
			flag = O_WRONLY | O_CREAT | O_APPEND;
		else
			flag = O_WRONLY | O_CREAT | O_TRUNC;
		fd = ft_err(open(*file, flag, 0666), "open error");
		id = 1;
	}
	unset(&fd[id]);
	fd[id] = fd;
	return (fd < 0);
}

int	redir_to_fd(t_node *node, int *fd, int *pfd)
{
	char	**file;
	t_node	*iter;
	int		flag;

	fd[0] = 0;
	fd[1] = 1;
	iter = node->redir_next;
	flag = 0;
	while (iter && !flag)
	{
		file = expand_all(iter->redir_target);
		if (!file || shell_assert((file[1] != NULL), "ambiguous redirect"))
			return (1);
		flag = update_redir_fd(fd, file, iter->redir_op->str);
		split_cleanup(file);
		iter = iter->redir_next;
	}
	return (select_fd(fd, pfd, flag));
}
