/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 21:19:28 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/19 21:19:30 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	check_terminal(struct termios *term, char *err_str)
{
	int	cond[2];

	cond[0] = !ft_err(tcgetattr(0, &term[2]), err_str);
	cond[1] = 1;
	if (!cond[0])
	{
		cond[1] = ft_memcmp(&term[1], &term[2], sizeof(struct termios));
		shell_assert(cond[1], err_str);
	}
	return (cond[1]);
}

int	query_terminal(char *err_str)
{
	size_t	i;
	size_t	k;
	char	*query;

	query = "/033[6n";
	i = 0;
	while (i < ft_strlen(query))
	{
		k = ft_err(write(1, &query[i], 4 - i), err_str);
		if (k < 0 && errno != EINTR)
			return (1);
		i += k;
	}
	return (0);
}

//do not consider sigint
int	check_nl(char *err_str)
{
	int		read_col;
	int		col;
	int		len;
	int		done;
	char	buf;

	done = 0;
	read_col = 0;
	col = 0;
	if (query_terminal(err_str))
		return (0);
	while (!done)
	{
		len = ft_err(read(0, &buf, 1), err_str);
		if ((len < 0 && errno != EINTR) || !len || buf == 'R')
			break ;
		else if (!read_col && buf != ';')
			continue ;
		read_col = 1;
		col = (col * 10) + (buf - '0');
	}
	return (col);
}

void	replace_nl(void)
{
	struct termios	term[3];
	char			*err_str;
	int				result;

	err_str = "error while checking terminal display";
	if (ft_err(tcgetattr(0, &term[0]), err_str))
		return ;
	term[1] = term[0];
	term[1].c_lflag &= (~ICANON & ~ECHO);
	term[1].c_cc[VMIN] = 0;
	term[1].c_cc[VTIME] = 1;
	result = 0;
	if (ft_err(tcsetattr(0, TCSANOW, &term[1]), err_str))
		return ;
	else if (!check_terminal(term, err_str))
		result = check_nl(err_str);
	ft_err(tcsetattr(0, TCSANOW, &term[0]), "error restoring terminal");
	if (result > 1)
		ft_putchar('\n');
}
