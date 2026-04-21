# include "h_minishell.h"

void	cmd_pop(t_cmd **cmd)
{
	t_cmd	*next;

	if (*cmd)
	{
		next = (*cmd)->next;
		cmd_delone(*cmd);
		*cmd = next;
	}
}

//would debug if we used this system
t_cmd	*cmdtrim(t_cmd **list, t_cmd *head, t_cmd *tail)
{
	t_cmd	*prev;
	t_cmd	*next;

	prev = *list;
	while (prev != head && prev->next != head)
		prev = prev->next;
	next = tail->next;
	tail->next = NULL;
	if (prev != head)
		prev->next = next;
	else
		*list = next;
	return (head);
}
