int	select_fd(int *fd, int *pfd, int flag)
{
	//if flag, close all fds
	//else, set_fd
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
		file = expand_env(iter->redir_target);
		if (!file || shell_assert((file[1] != NULL), "ambiguous redirect"))
			return (1);
		flag = update_redir_fd(fd, file, iter->redir_op);
		split_cleanup(file);
		iter = iter->redir_next;
	}
	return (select_fd(fd, pfd, flag));
}
