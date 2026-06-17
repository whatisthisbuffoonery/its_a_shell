/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:47:41 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:47:41 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

char	*longest_argv(char *path, char *src)
{
	char	*ret;
	int		max;
	int		i;
	int		k;

	max = 0;
	i = 0;
	while (path[i])
	{
		k = 0;
		while (path[i + k] && path[i + k] != ':')
			k ++;
		if (k > max)
			max = k;
		i += k + (path[i + k] != '\0');
	}
	ret = malloc(max + ft_strlen(src) + 1 + 1);
	return (ret);
}

void	mark_assignment_args(t_tok *argv)
{
	t_tok	*arg;

	if (!argv)
		return ;
	if (ft_strcmp(argv->str, "export") != 0)
		return ;
	arg = argv->next;
	while (arg)
	{
		if (is_assignment_word(arg->str))
			arg->assignment = 1;
		arg = arg->next;
	}
}

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

void	process_argv(char **argv, t_tok *src)
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

char	**make_argv(t_tok *src, t_env *env, int *complain)
{
	char		**argv;
	char		*cmd;
	t_shnode	*path;
	int			status;

	status = 1;
	mark_assignment_args(src);
	argv = expand_all(src, env, collect_argv);
	process_argv(argv, src);
	if (!argv || !argv[0])
		return (argv);
	path = find_env("PATH", env->env);
	if (ft_strchr(*argv, '/') || isbuiltin(*argv)
		|| !path || !path->str || !path->str[0])
		return (argv);
	cmd = longest_argv(path->str, *argv);
	if (ft_err(-!cmd, "cmd malloc error"))
		return (split_cleanup(argv));
	find_path(path->str, argv, cmd, &status);
	if (status && status != 1)
		*complain = status;
	else if (status)
		*complain = ENOENT;
	return (argv);
}
