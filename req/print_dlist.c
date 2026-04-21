#include "h_minishell.h"

void	print_dlist(t_dlist *dlist, int depth)
{
	if (dlist->down)
	{
		ft_printf("( open %d\n ", depth + 1);
		print_dlist(dlist->down, depth + 1);
		ft_printf("close %d )\n", depth + 1);
	}
	if (dlist->down_next)
	{
		ft_printf("( open %d\n ", depth);
		print_dlist(dlist->down_next, depth);
		ft_printf("close %d )\n", depth);
	}
	print_linear_cst(dlist->cst);
	if (dlist->redir)
		print_linear_cmd(dlist->redir, "subshell redir");
	if (dlist->across)
	{
		ft_printf("across %d\n", depth);
		print_dlist(dlist->across, depth);
	}
}

void	print_dlist_digestible(t_dlist *dlist)
{
	if (dlist->down)
	{
		ft_putstr("( ");
		print_dlist_digestible(dlist->down);
		ft_putstr(") ");
	}
	if (dlist->down_next)
	{
		ft_putstr("( ");
		print_dlist_digestible(dlist->down_next);
		ft_putstr(") ");
	}
	if (dlist->cst)
	{
		if (dlist->cst->op)
			print_linear_cmd(dlist->cst->op, NULL);
		print_linear_cmd(dlist->cst->cmd, NULL);
	}
	if (dlist->across)
		print_dlist_digestible(dlist->across);
}
