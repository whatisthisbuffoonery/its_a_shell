#include "h_minishell.h"

int	isempty(char *buf)
{
	int	i;

	if (!buf)
		return (1);
	i = 0;
	while (buf[i] && ft_isspace(buf[i]))
		i ++;
	return (!buf[i]);
}

int	quote_check(char *buf)
{
	int	c;
	int	i;

	i = 0;
	c = 0;
	while (buf[i])
	{
		if (!c && ft_isquote(buf[i]))
			c = buf[i];
		else if (c && buf[i] == c)
			c = 0;
		i ++;
	}
	return (shell_assert(c, "unclosed quotes"));
}

int	buf_check(char *buf)
{
	return (isempty(buf) || quote_check(buf));
}