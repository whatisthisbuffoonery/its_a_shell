#include "h_minishell.h"

static int	ft_iscarrot(int c)
{
	return (c == '>' || c == '<');
}

static int	mand_cond(char c, char i)
{
	if (!c || !i)
		return (0);
	if (c == '|' || ft_isquote(c) || ft_iscarrot(c))
		return (0);
	if (i == '|' || ft_isquote(i) || ft_iscarrot(i))
		return (0);
	return (1);
}

//this splits words, quotes, and operators &, |, >, <

//splitting words from quotes is done for simplicity,
//but should be recombined if they were not separated by whitespace

//check for ending whitespace, ls'>'wa should stay as one element
int	tok_node_init(t_tok **dst, char *src, int *cry)
{
	int		i;
	char	c;
	t_tok	*ret;

	i = 0 + 1;
	c = src[0];
	while (!g_signo && (((c == '|' || c == '*') && i < 1)
			|| (ft_iscarrot(c) && c == src[i] && i < 2)
			|| mand_cond(c, src[i])
			|| (ft_isquote(c) && src[i] && src[i] != c)))
		i ++;
	ret = tok_node(src, i, c, cry);
	tok_node_append(dst, ret);
	return (i + hadquote(src, i));
}

int	simple_cond(t_tok *src)
{
	if (!src || !src->type || !ft_strcmp(src->str, "|"))
		return (0);
	return (1);
}

t_node_kind	find_kind_op(t_tok *tok)
{
	(void) tok;
	return (N_CMD);
}

int	group_kind(char c)
{
	(void) c;
	return (0);
}
