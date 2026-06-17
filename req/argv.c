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

void	path_error(char *new, int *status)
{
	struct stat	dump;

	if (errno == ENOENT || stat(new, &dump))
		return ;
	ft_err(-1, new);
	*status = errno;
	(void) dump;
}

int	empty_path(char *dst, char *new, int *status)
{
	if (dst[0])
		return (0);
	free(new);
	*status = ENOENT;
	return (1);
}

void	find_path(char *path, char **dst, char *new, int *status)
{
	int		i;
	int		k;

	i = 0;
	if (empty_path(*dst, new, status))
		return ;
	while (path[i])
	{
		k = 0;
		while (path[i + k] && path[i + k] != ':')
			k ++;
		ft_strlcpy(new, &path[i], k + 1);
		ft_strlcat(new, "/", -1);
		ft_strlcat(new, *dst, -1);
		if (!access(new, X_OK))
		{
			*status = 0;
			free(*dst);
			*dst = new;
			return ;
		}
		path_error(new, status);
		i += k + (path[i + k] != '\0');
	}
	free(new);
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

char	**make_argv(t_tok *src, t_env *env, int *complain)
{
	char		**argv;
	char		*cmd;
	t_shnode	*path;
	int			status;

	status = 1;
	mark_assignment_args(src);
	argv = expand_all(src, env, collect_argv);
	if (!argv)
		return (NULL);
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
