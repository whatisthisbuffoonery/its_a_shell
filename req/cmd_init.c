#include "h_minishell.h"

int	ft_crutch(char *s, int n)
{
	return (ft_isquote(s[0]) != 0 && s[n] == s[0]);
}

t_cmd	*cmd_node(char *src, int i, char c, int *cry)
{
	t_cmd	*ret;

	ret = ft_calloc(sizeof(t_cmd), 1);
	if (!ret)
	{
		ft_err(-1, "cmd node malloc");
		return (NULL);
	}
	shell_assert(!i || !c, "zero string???? how???????\n");
	//ft_printf("debug: %c:%d, %c:%d\n", src[0], src[0], src[i], src[i]);
	ret->str = ft_substr(src, ft_crutch(src, i), i - ft_crutch(src, i));//src[0] is a quote if it is a quote section, flag adds the other quote
//	ft_printf("substr: %d, bool: %d, str: %s\n", i, ft_crutch(src, i), ret->str);
	if (!ret->str)
		*cry = (ft_err(-1, "cmd node str malloc"));
	ret->type = c;
	if (ret->str && !ft_strcmp(ret->str, "&"))//single & not required
		ret->type = "me is name node"[0];
	ret->end_space = ft_isspace(src[i + ft_crutch(src, i)]);//bool
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
int	cmd_node_init(t_cmd **dst, char *src, int *cry)
{
	int		i;
	char	c;
	t_cmd	*ret;

	i = 1;//oh mah gah
	c = src[0];
	while (!muh_number && ((isop(c) && src[i] == c && i < 2)			//operator
		|| (iscontent(c) && iscontent(src[i]))							//operand
			|| (ft_isquote(c) && src[i] && src[i] != c)					//quote, also operand
				|| ((isbracket(c) || c == '*') && i < 1)))				//put brackets in their own node
		i ++;
	ret = cmd_node(src, i, c, cry);
	cmd_node_append(dst, ret);
	return (i + ft_crutch(src, i));//thing //replaced c with ret str//WARNING//put src now
}

int	cmd_init(char *buf, t_cmd **cmd)
{
	int	i;
	int	cry;

	i = 0;
	cry = 0;
	while (buf && buf[i])
	{
		while (ft_isspace(buf[i]))
			i ++;
		if (!buf[i])
			break ;
		i += cmd_node_init(cmd, &buf[i], &cry);
		if (cry)
			return (1);
	}
	return (0);
}
