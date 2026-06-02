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

//if flag, close all fds
//else, set_fd
int	select_fd(int *fd, int *pfd, int flag)
{
	ft_printf("select fd entry: [%d, %d], flag: %d\n", fd[0], fd[1], flag);
	if (flag)
	{
		unset(&fd[0]);
		unset(&fd[1]);
		unset(&pfd[0]);
		unset(&pfd[1]);
	}
	else
		set_fd(fd, pfd);
	ft_printf("select fd: [%d, %d]\n", fd[0], fd[1]);
	return (flag);
}

int	heredoc_cmp(char *line, char *v, int v_len)
{
	int	line_len;

	ft_printf("debug: v: [%s] len: %d, cmp: [%s] len: %d\n", v, v_len, line, line ? ft_strlen(line):0);
	if (!line || signo)//allow sigquit outcome here
		return (0);
	line_len = ft_strlen(line);
	if (line_len != v_len)//+ 1 || line[v_len] != '\n')
		return (1);
	return (ft_strncmp(line, v, v_len));
}

int	heredoc_help(char *s, int fd)
{
	int	i;

	free(s);
	i = write(fd, "\n", 1);
	while (i < 0 && errno == EINTR && !signo)
		i = write(fd, "\n", 1);
	return (ft_err(i, "write error") < 0 || signo != 0);
}

int	heredoc_write(int *fd, char *s, t_env *env, int flag)
{
	int		i;
	int		k;
	int		len;
	t_tok	tmp;

	if (flag && !signo)
	{
		tmp.str = s;
		if (use_expansion(&tmp, env, NULL))
		{
			unset(&fd[0]);
			return (1);
		}
		s = tmp.str;
	}
	len = ft_strlen(s);
	i = 0;
	while (i < len && !signo)
	{
		k = write(fd[1], s, len - i);
		if (ft_err(-(k < 0 && errno != EINTR), "write error"))
			return (1);
		i += k + (k < 0);
	}
	return (heredoc_help(s, fd[1]));
}

int	rl_heredoc(void)
{
	if (signo == SIGQUIT)
		signo = 0;
	else if (signo == SIGINT)
	{
		rl_done = 1;
		ft_putstr("^C");
	}
	return (0);
}

int	do_heredoc(char *file, t_env *env, int flag)
{
	int		fd[2];
	char	*buf;
	int		v_len;

	if (ft_err(pipe(fd), "pipe error"))
		return (-1);
	errno = 0;
	buf = NULL;
	v_len = ft_strlen(file);
	rl_signal_event_hook = rl_heredoc;
	while (!signo)
	{
		buf = readline("> ");//I shit you not //should ignore sigquit
		if(!heredoc_cmp(buf, file, v_len) || heredoc_write(fd, buf, env, flag))
			break ;
//		free(buf);
		buf = NULL;
	}
	free(buf);
	close(fd[1]);
	if (signo)
		unset(&fd[0]);
	rl_signal_event_hook = rl_handle_signals;
	return (fd[0]);
}

//allow empty string
char	**catch_heredoc(t_node *src, t_env *env, int (*f)(t_arg **, t_tok *))
{
	char	**file;

	if (ft_strcmp(src->redir_op->str, "<<"))
		return (expand_all(src->redir_target, env, f));
	file = NULL;
	if (!malloc_cond((void **) &file, 2 * sizeof(char *))
		|| !word_to_str(file, src->redir_target))
	{
		free(file);
		return (NULL);
	}
	file[1] = NULL;
	return (file);
}

//no need to iterate here 
int	update_redir_fd(int *fd, char **file, t_node *iter, t_env *env)
{
	int		id;
	int		new_fd;
	char	*op;

	op = iter->redir_op->str;
	id = (op[0] == '>');
	if (!id && op[1])
		new_fd = do_heredoc(*file, env, iter->heredoc);//to change
	else if (!id)
		new_fd = open(*file, O_RDONLY);
	else if (id && op[1])
		new_fd = open(*file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		new_fd = open(*file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	
	unset(&fd[id]);
	fd[id] = new_fd;
	ft_printf("update_redir: %d at fd[%d], cond: %d || %d, cond actual: %d\n", new_fd, id, signo != 0, new_fd < 0, signo != 0 || ft_err(new_fd, "") < 0);
	return (signo != 0 || ft_err(new_fd, "open error") < 0);
}

int	find_quote(t_tok *tok)
{
	while (tok)
	{
		if (ft_isquote(tok->type))
			return (1);
		tok = tok->word_next;
	}
	return (0);
}

char	*redir_err_string(t_node *iter)
{
	if (!iter->redir_target || !iter->redir_target->str)
		return ("(null)");
	return (iter->redir_target->str);
}

int	redir_to_fd(t_node *node, t_env *env, int *fd, int *pfd)
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
		iter->heredoc = !find_quote(iter->redir_target);
		file = catch_heredoc(iter, env, collect_redir);
		if (!file || shell_assert_redir((!file[0] || file[1]
			|| (!file[0][0] && !ft_strcmp(iter->redir_target->str, "<<"))),
			iter->redir_target, "ambiguous redirect"))
		{
			split_cleanup(file);
			return (1);
		}
		ft_printf("schizo prior: %d\n", flag);
		flag = update_redir_fd(fd, file, iter, env);
		ft_printf("schizo: %d\n", flag);
		split_cleanup(file);
		iter = iter->redir_next;
	}
	return (select_fd(fd, pfd, flag));
}
