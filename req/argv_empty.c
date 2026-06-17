#include "h_minishell.h"

void	move_argv(char **argv)
{
	char	*tmp;
	int		i;

	i = 0;
	tmp = *argv;
	while (argv[i])
	{
		argv[i] = argv[i + 1];
		i ++;
	}
	free(tmp);
}

int	argv_check_quote(t_tok *src)
{
	while (src)
	{
		if (ft_isquote(src->type))
			return (1);
		src = src->word_next;
	}
	return (0);
}

void	argv_remove_empty(char **argv, t_tok *src)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		if (!argv[i][0] && !argv_check_quote(src))
		{
			move_argv(&argv[i]);
			src = src->next;
			continue ;
		}
		i ++;
	}
}
