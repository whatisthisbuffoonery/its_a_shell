#include "h_minishell.h"

void	print_dlist(t_dlist *dlist, int depth)
{
	if (dlist->down)
	{
		ft_printf("( open %d\n ", depth);
		print_dlist(dlist->down, depth + 1);
		ft_printf("close %d )\n", depth);
	}
	print_linear_cst(dlist->cst);
	if (dlist->across)
	{
		ft_printf("across %d\n", depth);
		print_dlist(dlist->across, depth);
	}
}
