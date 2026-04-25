#include "h_minishell.h"

int	ft_crutch(char *s, int n)
{
	return (ft_isquote(s[0]) != 0 && s[n] == s[0]);
}

t_tok	*tok_node(char *src, int i, char c, int *cry)
{
	t_tok	*ret;

	ret = ft_calloc(sizeof(t_tok), 1);
	if (!ret)
	{
		*cry = 1;
		ft_err(-1, "tok node malloc");
		return (NULL);
	}
	shell_assert(!i || !c, "zero string???? how???????\n");
	ret->str = ft_substr(src, ft_crutch(src, i), i - ft_crutch(src, i));
	if (!ret->str)
		*cry = (ft_err(-1, "tok node str malloc"));
	ret->type = c;
	if (ret->str && !ft_strcmp(ret->str, "&"))
		ret->type = "me is name node"[0];
	ret->end_space = ft_isspace(src[i + ft_crutch(src, i)]);
	return (ret);
}

void	tok_node_append(t_tok **dst, t_tok *src)
{
	t_tok	*iter;

	iter = *dst;
	while (iter && iter->next)
		iter = iter->next;
	if (!iter)
		*dst = src;
	else
		iter->next = src;
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

	i = 1;//oh mah gah
	c = src[0];
	while (!muh_number && ((isop(c) && src[i] == c && i < 2)			//operator
		|| (iscontent(c) && iscontent(src[i]))							//operand
			|| (ft_isquote(c) && src[i] && src[i] != c)					//quote, also operand
				|| ((isbracket(c) || c == '*') && i < 1)))				//put brackets in their own node
		i ++;
	ret = tok_node(src, i, c, cry);
	tok_node_append(dst, ret);
	return (i + ft_crutch(src, i));//thing //replaced c with ret str//WARNING//put src now
}

int	tok_init(char *buf, t_tok **tok)
{
	int	i;
	int	cry;

	i = 0;
	cry = 0;
	while (buf && buf[i] && !cry)
	{
		while (ft_isspace(buf[i]))
			i ++;
		if (!buf[i])
			break ;
		i += tok_node_init(tok, &buf[i], &cry);
		if (cry)
			return (1);
	}
	return (0);
}
