#include "h_minishell.h"

void	make_word(t_cmd *iter)
{
	t_cmd	*next;

	while (iter)
	{
		if (isjoined(iter))
		{
			iter->word_next = subcmd(&iter->next, isjoined);//append and update
			next = iter->word_next;
			while (next)
			{
				next->word_next = next->next;
				next->next = NULL;
				next = next->word_next;
			}
		}
		iter = iter->next;
	}
}

void	print_word(t_cmd *tok)
{
	if (!tok)
		return ;
	ft_putstr("[");
	ft_putstr(tok->str);
	while (tok->word_next)
	{
		tok = tok->word_next;
		ft_putstr(tok->str);
	}
	ft_putstr("]");
}
