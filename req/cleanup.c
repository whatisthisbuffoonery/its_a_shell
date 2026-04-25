#include "h_minishell.h"

void	clean_shnode(t_shnode **shnode)
{
	t_shnode	*iter;
	t_shnode	*next;

	iter = *shnode;
	while (iter)
	{
		next = iter->next;
		free(iter->name);
		free(iter->str);
		free(iter);
		iter = next;
	}
	*shnode = NULL;
}

void	clean_shnode_dup(t_shnode **shnode)
{
	t_shnode	*iter;
	t_shnode	*next;

	iter = *shnode;
	while (iter)
	{
		next = iter->next;
		free(iter);
		iter = next;
	}
	*shnode = NULL;
}

void	tok_delone(t_tok *tok)
{
	free(tok->str);
	clean_shnode_dup(&tok->env);
	free(tok);
}

void	clean_tok(t_tok **tok)
{
	t_tok	*iter;
	t_tok	*next;

	iter = *tok;
	while(iter)
	{
		next = iter->next;
		clean_tok(&iter->word_next);
		tok_delone(iter);
		iter = next;
	}
	*tok = NULL;
}

void	clean_ast(t_node *node)
{
	if (!node)
		return ;
	clean_tok(&node->argv);
	clean_tok(&node->redir_op);
	clean_tok(&node->redir_target);
	clean_ast(node->redir_next);
	clean_ast(node->left);
	clean_ast(node->right);
	free(node);
}
