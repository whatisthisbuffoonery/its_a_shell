#include "h_minishell.h"

int	do_cmd(t_cmd **head)
{
	t_cmd	*iter;
	int		cond;

	iter = head;
}

void	setup_matching(char b[], int (*ret[])(t_cmd *, int *))
{
	b[0] = "echo";
	b[1] = "cd";
	b[2] = "pwd";
	b[3] = "export";
	b[4] = "unset";
	b[5] = "env";
	b[6] = "exit";
	b[7] = NULL;
	ret[0] = echo;
	ret[1] = cd;
	ret[2] = pwd;
	ret[3] = export;
	ret[4] = unset;
	ret[5] = env;
	ret[6] = exit;
	ret[7] = NULL;
}

int	isbuiltin(t_cmd *cmd, int (**dst)(t_cmd *, int *))//number of asterisks for dst in question
{
	int			i;
	char		*s;
	static char	*b[8];
	static int	(*ret[8])(t_cmd *, int *);

	if (!b[0])
		setup_matching(b, ret);
	i = 0;
	s = cmd->str;
	while (b[i])
	{
		if (!ft_strcmp(s, b[i]))
		{
			if (dst)
				*dst = ret[i];
			return (1);
		}
		i ++;
	}
	return (0);
}


void	cmd_child(t_cmd *iter, int fd[])//can call builtins, should be called on either binaries or pipelines, not builtins outside of pipelines
{
	t_cmd	*cmd;
	int		result;
	int		(*builtin)(t_cmd *, int *);

	cmd = NULL;
	builtin = NULL;
	if (iscond(iter->type))
		iter = iter->next;
	while (iter && !iscond(iter->type))
	{
		if (isredir(iter->type) && make_redir(&iter, fd))
			exit(1);					//todo: replace exit funcs with cleanup, use static t_env and t_cmd fields
		else if (!cmd)
			cmd = iter;
		iter = iter->next;
	}
	if (isbuiltin(cmd, &builtin))
		result = builtin(cmd, fd);
	else
		result = binary(cmd, fd);
	exit(result);
}

//assumes we got rid of single & already
int	do_cmd(t_cmd **head, int last)
{
	t_cmd	*iter;

	if ((!last && iter->type == '&') || (last && !ft_strcmp(iter->str, "||"))
		|| iscontent(iter->type))
		last = fork_wrapper(*head);
	while (iter && (iscontent(iter->type) || isredir(iter->type)))
		iter = iter->next;
	*head = iter;
	if (iter)
		*head = iter->next;
	return (last);
}

//need to clean and update env lists on fork, which happens in here
int	cmd_parse(t_cmd **cmd, t_env *env)
{
	t_ast	ast;//bruh

	if (!*cmd)
		return (0);
}
