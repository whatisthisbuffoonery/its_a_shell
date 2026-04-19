/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 21:19:48 by achew             #+#    #+#             */
/*   Updated: 2026/04/19 21:26:32 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

static void free_redirs(t_node *r)
{
    t_node  *next;

    while (r)
    {
        next = r->redir_next;
        free(r->redir_op);
        free(r->redir_target);
        free(r);
        r = next;
    }
}

void    ast_free(t_node *n)
{
    int i;

    if (!n)
        return ;
    if (n->kind == N_CMD)
    {
        i = 0;
        while (n->argv && n->argv[i])
            free(n->argv[i++]);
        free(n->argv);
        free_redirs(n->redir_next);
    }
    else if (n->kind == N_GROUP)
    {
        ast_free(n->left);
        free_redirs(n->redir_next);
    }
    else if (n->kind == N_PIPE || n->kind == N_AND || n->kind == N_OR)
    {
        ast_free(n->left);
        ast_free(n->right);
    }
    free(n);
}
