#include "h_minishell.h"

//cmd word idea: start is head, word_next = subcmd(&head->next, isjoined)
t_node	*parse_simple(t_tok **tok, int *stop)
{
	t_node	*new_cmd;

	if (!*tok || !(*tok)->type || ismeta(*tok))//accept redir
	{
		ft_putstr_fd("expected command, got ", 2);
		if (!*tok)
			ft_putstr_fd("EOF", 2);
		else
			ft_putstr_fd((*tok)->str, 2);
		ft_putstr_fd("\n", 2);
		*stop = 1;
		return (NULL);
	}
	new_cmd = node_new(N_CMD, stop);//does not increment
	if (!new_cmd)
		return (NULL);
	while (*tok && !*stop && (*tok)->type && !ismeta(*tok))
	{
		if (isredir((*tok)->type))
			redir_append(new_cmd, parse_one_redir(tok, stop));//make a func
		else
			tok_node_append(&new_cmd->argv, subtok(tok, single_tok));//increment
	}
	return (new_cmd);
}

//veto open bracket check
t_node	*parse_group(t_tok **tok, int *stop)
{
	t_node	*new_group;

	tok_pop(tok);
	new_group = node_new(N_GROUP, stop);
	if (!new_group)
		return (NULL);
	new_group->left = parse_list(tok, stop);
	if (*stop)
		return (new_group);
	if (!*tok || (*tok)->type != ')')
	{
		ft_putstr_fd("syntax error: unclosed brackets\n", 2);
		*stop = 1;
		return (new_group);
	}
	tok_pop(tok);
	new_group->redir_next = parse_redir_group(tok, stop);
	return (new_group);
}

t_node	*parse_command(t_tok **tok, int *stop)
{
	if (!*tok)
	{
		ft_putstr_fd("syntax error: unexpected end of input\n", 2);
		*stop = 1;
		return (NULL);
	}
	if ((*tok)->type == '(')
		return (parse_group(tok, stop));
	return (parse_simple(tok, stop));
}

//left = pipe symbol, right = command
//execution idea: recursive pipe manager increments count
t_node	*parse_pipeline(t_tok **tok, int *stop)
{
	t_node	*L;
	t_node	*R;
	t_node	*new_pipe;

	L = parse_command(tok, stop);
	if (!L || *stop)
		return (L);
	while (*tok && !*stop && !ft_strcmp((*tok)->str, "|"))
	{
		new_pipe = node_new(N_PIPE, stop);
		if (!new_pipe)
			return (L);
		tok_pop(tok);
		R = parse_command(tok, stop);//veto failure catch
		new_pipe->left = L;
		new_pipe->right = R;
		L = new_pipe;
	}
	return (L);
}

t_node	*parse_list(t_tok **tok, int *stop)
{
	t_node	*L;
	t_node	*R;
	t_node	*new_op;

	L = parse_pipeline(tok, stop);
	if (!L || *stop)
		return (L);
	while (*tok && !*stop && ast_iscond(*tok))
	{
		new_op = node_new(find_kind_op(*tok), stop);
		if (!new_op)
			return (L);
		tok_pop(tok);
		R = parse_pipeline(tok, stop);//veto failure catch
		new_op->left = L;
		new_op->right = R;
		L = new_op;
	}
	return (L);
}
