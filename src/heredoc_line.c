#include "h_minishell.h"

void	heredoc_realloc(char **line, int len, int *done)
{
	char	*ret;
	int		i;

	ret = malloc(len + 32);
	if (!ret)
	{
		*done = -1;
		return ;
	}
	i = 0;
	while (i < len)
	{
		ret[i] = (*line)[i];
		i ++;
	}
	free(*line);
	*line = ret;
}

char	*heredoc_line_finish(char *line, int done, int i)
{
	if (done > 0)
		line[i] = '\0';
	if (done < 0 || !i)
	{
		free(line);
		line = NULL;
	}
	if (!i)
		ft_putchar('\n');
	return (line);
}

char	*heredoc_line(void)
{
	char	*line;
	char	buf;
	int		i;
	int		len;
	int		done;

	i = 0;
	done = 0;
	buf = '\0';
	if (!malloc_cond((void **) &line, 32))
		return (NULL);
	ft_putstr("> ");//only refresh on seeing nl
	while (!done)
	{
		len = read(0, &buf, 1);
		if (!len || (len > 0 && buf == '\n'))
			done = 1;
		else if (len < 0 && (errno != EINTR || g_signo))
			done = -1;
		if (!done)
			line[i++] = buf;
		if (done >= 0 && i && !((i + 1) % 32))
			heredoc_realloc(&line, i + 1, &done);
	}
	return (heredoc_line_finish(line, done, i));
}
