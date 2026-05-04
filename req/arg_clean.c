#include "h_minishell.h"

void	free_arg(t_arg *arg)
{
	free(arg->str);
	free(arg->mask);
	free(arg);
}

t_arg	*free_arg_list(t_arg *head)
{
	t_arg	*next;
	
	while (head)
	{
		next = head->next;
		free_arg(head);
		head = next;
	}
	return (NULL);
}
