#include "h_minishell.h"

int	ft_crutch(char *s, int n)
{
	return (ft_isquote(s[0]) != 0 && s[n] == s[0]);
}

t_cmd	*cmd_node(char *src, int i, char c, int *cry)
{
	t_cmd	*ret;

	ret = malloc(sizeof(t_cmd));
	if (!ret)
	{
		ft_err(-1, "cmd node malloc");
		return (NULL);
	}
	shell_assert(!i || !c, "zero string???? how???????\n");
	ret->str = ft_substr(src, 0, i + (1 * ft_crutch(src, i)));//src[0] is a quote if it is a quote section, flag adds the other quote
//	ft_printf("substr: %d, bool: %d, str: %s\n", i, ft_crutch(src, i), ret->str);
	ret->next = NULL;
	ret->env = NULL;
	ret->type = '\0';
	if (!ret->str)
		*cry = (ft_err(-1, "cmd node str malloc"));
	ret->type = c;
	if (ret->str && !ft_strcmp(ret->str, "&"))//single & not required
		ret->type = "me is name node"[0];
	ret->end_space = ft_isspace(src[i + (src[i] && ft_crutch(src, i))]);//bool
	return (ret);
}

void	cmd_node_append(t_cmd **dst, t_cmd *src)
{
	t_cmd	*iter;

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

//put sigint in a more convenient spot
int	node_init(t_cmd **dst, char *src, int *cry)
{
	int		i;
	char	c;
	t_cmd	*ret;

	i = 1;//oh mah gah
	c = src[0];
	ft_printf("what am i: %c:%d\n", c, c);
	while (/*!muh_number && */((isop(c) && src[i] == c && i < 2)		//operator
		|| (iscontent(c) && iscontent(src[i]))							//operand
			|| (ft_isquote(c) && src[i] && src[i] != c)					//quote, also operand
				|| ((isbracket(c) || c == '*') && i < 1)))				//put brackets in their own node
		i ++;
	ret = cmd_node(src, i, c, cry);
	ft_printf("make node: %s, len: %d, type: %c:%d\n", ret->str, i, c, c);
	cmd_node_append(dst, ret);
	return (i + ft_crutch(ret->str, i));//thing //replaced c with ret str
}
