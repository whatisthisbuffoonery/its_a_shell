#include "h_minishell.h"

int	ft_strcmp_wrapper(t_shnode *a, t_shnode *b)
{
	return (ft_strcmp(a->name, b->name));
}

static void	window_shopping(t_shnode *curr, t_shnode *a, t_shnode *b)
{
	t_shnode	*iter;

	while (a && b)
	{
		if (ft_strcmp_wrapper(a, b) < 0)
		{
			curr->next = a;
			a = a->next;
		}
		else
		{
			curr->next = b;
			b = b->next;
		}
		curr = curr->next;
	}
	iter = b;
	if (!b)
		iter = a;
	while (iter)
	{
		curr->next = iter;
		curr = curr->next;
		iter = iter->next;
	}
}

static void	merge_split(t_shnode **head, t_shnode **a, t_shnode **b)
{
	t_shnode	*i;
	t_shnode	*k;

	i = *head;
	k = NULL;
	*a = i;
	if (i)
		k = i->next;
	while (k && k->next)
	{
		i = i->next;
		k = k->next->next;
	}
	if (i)
		*b = i->next;
	if (*b)
		i->next = NULL;
}

void	merge_sort(t_shnode **head)
{
	t_shnode	*a;
	t_shnode	*b;

	a = NULL;
	b = NULL;
	merge_split(head, &a, &b);
	if (a && a->next)
		merge_sort(&a);
	if (b && b->next)
		merge_sort(&b);
	if (b && ft_strcmp_wrapper(a, b) > 0)
	{
		*head = b;
		b = b->next;
	}
	else
	{
		*head = a;
		a = a->next;
	}
	window_shopping(*head, a, b);
}
