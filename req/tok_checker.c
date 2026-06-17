#include "h_minishell.h"

int	isredir(int c)
{
	return (c == '>' || c == '<');
}

int	isbracket(int c)
{
	return (c == '(' || c == ')');
}

int	iscond(int c)
{
	return (c == '|' || c == '&');
}

int	isop(int c)
{
	return (isredir(c) || iscond(c));
}

/*&& c != '='  do not consider */
int	iscontent(int c)
{
	return (c && c != '*'
		&& !isop(c) && !ft_isquote(c) && !ft_isspace(c) && !isbracket(c));
}
