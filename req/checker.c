#include "h_minishell.h"

int	isredir(int c)
{
	return (c == '>' || c == '<');
}

int	isbracket(int c)//move this over to libft
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

int	isenv(char c)
{
	//ft_printf("%d\n", c);
	return (c == '_' || ft_isalnum(c));
}

int	envname(char *s)
{
	return (s[0] == '$' && isenv(s[1]));
}

int	iscontent(int c)
{
	ft_printf("iscontent: %c:%d\n", c, c);
	return (c && c != '*' && !isop(c) && !ft_isquote(c) && !ft_isspace(c) && !isbracket(c));
}
