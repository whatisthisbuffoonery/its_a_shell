
/*
#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <errno.h>
#include "libft.h"
*/
#include "h_minishell.h"

volatile sig_atomic_t	muh_number;

int	ft_err(int n, char *s)
{
	if (n < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(s);
	}
	return (n);
}

int	isredir(int c)
{
	return (c == '>' || c == '<');
}

int	isbracket(int c)
{
	return (c == '(' || c == ')');
}

int	iscond(int c)
{
	return (c == '|' || c == '&');
}

int	isop(int c)
{
	return (isredir(c) || iscond(c));
}

int	iscontent(int c)
{
	return (c && c != '*' && !isop(c) && !ft_isquote(c) && !ft_isspace(c) && !isbracket(c));
}

t_cmd	*cmd_node(char *src, int i, char c, int *cry)
{
	t_cmd	*ret;

	ret = malloc(sizeof(t_cmd));
	if (!ret)
	{
		err(-1, "cmd node malloc");
		return (NULL);
	}
	ret->str = ft_substr(src, 0, i + (2 * (ft_isquote(c) != 0)));
	ret->next = NULL;
	ret->env = NULL;
	ret->type = '\0';
	if (!ret->str)
		*cry = (err(-1, "cmd node str malloc"));
	ret->type = c;
	if (ret->str && !ft_strcmp(ret->str, "&"))//single & not required
		ret->type = '@';
	ret->end_space = ft_isspace(src[i + (1 * (src[i] && ft_isquote(c)))]);//bool
	return (ret);
}

void	cmd_node_append(t_cmd **dst, t_cmd *ret)
{
	t_cmd	*iter;

	iter = *dst;
	while (iter && iter->next)
		iter = iter->next;
	if (!iter)
		*dst = ret;
	else
		iter->next = ret;
}

//this splits words, quotes, and operators &, |, >, <

//splitting words from quotes is done for simplicity,
//but should be recombined if they were not separated by whitespace

//check for ending whitespace, ls'>'wa should stay as one element
int	node_init(t_cmd **dst, char *src, int *cry)
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
	return (i + (1 * (ft_isquote(c) != 0)));//thing
}


/*merge sort section-------------------------------------------------------------------------------*/

int	shnode_strlen(t_shnode *env)
{
	if (env && env->str)
		return (ft_strlen(env->str));
	return (0);
}

t_shnode	*find_env(char *str, t_shnode *list, int len)
{
	unsigned int	n;

	n = (unsigned int) len;
	while (list && (ft_strlen(list->name) != n || ft_strncmp(str, list->name, n)))
		list = list->next;
	return (list);
}

t_shnode	*expansion_dup(t_shnode *src)
{
	t_shnode	*ret;

	if (!src)
		return (NULL);
	ret = malloc(sizeof(t_shnode));
	if (!ret)
	{
		err(-1, "expansion malloc");
		return (NULL);
	}
	ret->name = src->name;
	ret->str = src->str;
	ret->next = NULL;
	return (ret);
}
int	env_add(t_env *env, t_shnode *src, char *dst)
{
	t_shnode	*ret;
	t_shnode	*iter;
	t_shnode	**list;

	ret = src;
	list = &env->export;
	if (dst[1] == 'n')
	{
		ret = malloc(sizeof(t_shnode));
		if (!ret)
			return (err(-1, "env dup malloc"));
		ret->name = src->name;
		ret->str = src->str;
		list = &env->env;
	}
	if (ret)
		ret->next = NULL;
	iter = *list;
	while (iter && iter->next)
		iter = iter->next;
	if (!iter)
		*list = ret;
	else
		iter->next = ret;
	return (0);
}

void	shnode_append(t_shnode **dst, t_shnode *src)
{
	t_shnode	*iter;

	iter = *dst;
	while (iter && iter->next)
		iter = iter->next;
	if (iter)
		iter->next = src;
	else
		*dst = src;
}

/*shnode utilities------------------------------------------------------------*/

int	is_env(char c)
{
	return (c == '_' || ft_isalnum(c));
}

int add_expansion(t_cmd *dst, t_shnode *env, int *index)
{
	t_shnode	*ret;
	char		*str;
	int			i;

	i = 0;
	str = &dst->str[*index + 1];//dollar offset
	while (is_env(str[i]))
		i ++;
	*index += i + 1;//use env name len plus dollar
	if (find_env(str, dst->env, i))
		return (0);
	env = find_env(str, env, i);
	ret = expansion_dup(env);//mallocs a node, does not malloc the strings so dont free those
	if (!ret)
		return (1);
	shnode_append(&dst->env, ret);
	return (0);
}

void	copy_wrapper(char *src, char *dst, int *i, int *len)
{
	if (dst)
		dst[*len] = src[*i];
	*i += 1;
	*len += 1;
}

void	concat_wrapper(t_cmd *dst, char *ret, int *i, int *len)
{
	t_shnode	*iter;
	char		*str;
	int			k;
	int			tmp_len;

	k = 0;
	iter = dst->env;
	str = &dst->str[*i + 1];
	tmp_len = 0;
	while (is_env(str[k]))//huh
		k ++;
	iter = find_env(str, dst->env, k);
	if (ret && iter)
		ft_strlcat(ret, iter->str, -1);
	if (iter)
		tmp_len += ft_strlen(iter->str);
	*i += k + 1;
	*len += tmp_len;
	ft_printf("\n\nk: %d, tmp_len: %d\n\n", k, tmp_len);
}

int	use_expansion(t_cmd *dst, char *ret)
{
	int		i;
	int		len;

	i = 0;
	len = 0;
	if (ret)
		ret[0] = '\0';
	while (dst->str[i])
	{
		if (dst->str[i] == '$' && is_env(dst->str[i + 1]))
			concat_wrapper(dst, ret, &i, &len);//either strlen or strlcat
		else if (dst->str[i]
			&& (dst->str[i] != '$' || !is_env(dst->str[i + 1])))
			copy_wrapper(dst->str, ret, &i, &len);//copy one char//yes we copy dollar sign if env name is invalid
	}
	ft_printf("\nis null: %d, len: %d\n", !ret, len);
	if (!ret
		&& (!ft_err(-!malloc_cond((void **) &ret, len + 1), "expansion result malloc")))
		return (use_expansion(dst, ret));
	else if (!ret)
		return (1);
	free(dst->str);
	dst->str = ret;
	return (0);
}

int	expand_str(t_cmd **cmd, t_shnode *env)
{
	t_cmd	*iter;
	int		i;

	iter = *cmd;
	while(iter)
	{
		i = 0;
		while (iter->str[i] && iter->type != '\'')
		{
			if (iter->str[i] == '$' && is_env(iter->str[i + 1])
				&& add_expansion(iter, env, &i))
				return (1);
			i += (iter->str[i] && iter->str[i] != '$');
		}
		iter = iter->next;
	}
	iter = *cmd;
	while (iter)//move this over to command forking side, expand envs before each command, not before *all* commands
	{
		if (iter->type != '\'' && iter->env && use_expansion(iter, NULL))
			return (1);
		iter = iter->next;
	}
	return (0);
}
t_shnode	*env_init_node(char *e)
{
	t_shnode	*ret;
	int			i;

	ret = malloc(sizeof(t_shnode));
	if (!ret)
		return (NULL);
	i = 0;
	while (e[i] && e[i] != '=')
		i ++;
	ret->name = ft_strndup(e, i);
	if (ret->name)
		ret->str = ft_strdup(&e[i + (e[i] != '\0')]);
	if (!ret->name || !ret->str)
	{
		free(ret->name);
		free(ret);
		return (NULL);
	}
	ret->next = NULL;
	return (ret);
}

int	shell_assert(int cond, char *s)
{
	if (cond)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(s, 2);
	}
	return (cond);
}

//note: this func should only be called on making another child within minishell, bash hands off an updated shell lvl already
int	update_shell_lvl(t_env *dst)
{
	t_shnode	*iter;
	char		*ret;

	iter = find_env("SHLVL", dst->export, ft_strlen("SHLVL"));
	if (!iter)
	{
		iter = env_init_node("SHLVL=1");
		if (!iter)
			return (err(-1, "could not replace missing shlvl"));
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		return (0);
	}
	if (iter->str)
		ret = ft_itoa(ft_atoi(iter->str) + 1);
	else
		ret = ft_itoa(1);
	if (!ret)
		return (err(-1, "shlvl update error"));
	free(iter->str);
	iter->str = ret;
	iter = find_env("SHLVL", dst->env, ft_strlen("SHLVL"));
	if (!shell_assert(!iter, "shlvl missing in env"))
		iter->str = ret;
	return (0);
}

//technically you would getcwd() here to list the binary name and location
int	update_shell_name(t_env *dst)
{
	t_shnode	*iter;
	char		*ret;

	iter = find_env("SHELL", dst->export, ft_strlen("SHELL"));
	if (!iter)
	{
		iter = env_init_node("SHELL=minishell");
		if (!iter)
			return (err(-1, "could not replace missing shell name"));
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		return (0);
	}
	ret = ft_strdup("minishell");
	if (!ret)
		return (err(-1, "shell name update error"));
	free(iter->str);
	iter->str = ret;
	iter = find_env("SHELL", dst->env, ft_strlen("SHELL"));
	if (!shell_assert(!iter, "shell name missing in env"))
		iter->str = ret;
	return (0);
}
//init shell level, only init cd dash if null/not present
//also also change SHELL to be minishell, update SHLVL
//dealing with LINES and COLUMNS for display stuff is way outside subject scope
void	env_init(t_env *dst, char **e)
{
	int			i;
	t_shnode	*iter;

	i = 0;
	ft_memset(dst, 0, sizeof(t_env));
	while (e[i])
	{
		iter = env_init_node(e[i]);
		if (!iter && err((-!iter), "export node malloc"))
			break ;
		env_add(dst, iter, "env");
		env_add(dst, iter, "export");
		i ++;
	}
	merge_sort(&dst->env);
	update_shell_lvl(dst);
	update_shell_name(dst);
}
void	do_thing(char *buf)
{
	char *arr[3];
	pid_t pid;

	(void) buf;
	ft_putstr("<thing> ");
	arr[0] = "/usr/bin/sleep";
	arr[1] = "3";
	arr[2] = NULL;
	pid = fork();
	if (!pid)
	{
		execve(arr[0], arr, NULL);
		perror("huh");
		exit(67);
	}
	/*
	while (w)
	{
		pid = wait(NULL);
		errno = 0;
		flag -= (pid < 0);
		if (flag)
		{
			ft_putnbr(pid);		//sig int causes this to immediately return -1
			ft_putchar('\n');
		}
		if (errno || pid < 0)
		{
			perror("error");
		}
	}
	*/
	while (waitpid(pid, NULL, 0) < 0);
	ft_putstr("done\n");
}

char	*env_safe(char *s, char *n)
{
	if (!s)
		return (n);
	return (s);
}
void	print_env(t_shnode *env)
{
	while (env)
	{
		ft_printf("<%s, %s> ", env->name, env_safe(env->str, "NULL"));
		env = env->next;
	}
	ft_putstr("\n\n");
}

void	print_cmd(t_cmd **cmd, int *last)
{
	t_cmd	*iter;

	iter = *cmd;
	while (iter)
	{
		ft_printf("[%s]\n", iter->str);
		if (iter->env)
			print_env(iter->env);
		iter = iter->next;
	}
	ft_printf("\nexit status: %d\n", *last);
}

void	clean_cmd(t_cmd **cmd)
{
	t_cmd	*iter;
	t_cmd	*next;

	iter = *cmd;
	while(iter)
	{
		next = iter->next;
		free(iter->str);
		free(iter);
		iter = next;
	}
}

void	shell_print(char *buf, t_env *env)
{
	int	i = 0;
	int	cry = 0;
	t_cmd	*cmd = NULL;

	while (buf && buf[i])
	{
		while (ft_isspace(buf[i]))
			i ++;
		if (!buf[i])
			break ;
		i += node_init(&cmd, &buf[i], &cry);
		if (cry)
			return ;
	}
	if (buf && buf[0])
	{
		expand_str(&cmd, env->env);
		print_cmd(&cmd, &cry);
		add_history(buf);
		clean_cmd(&cmd);
	}
	else if (!buf)
		ft_putstr("NULL");
	ft_putchar('\n');
//	if (buf && buf[0] == 's' && !buf[1])
//		do_thing(buf);
	free(buf);
}

int	rl_handle_signals(void)//sigaction flag interrupt, rl handler	//does nl + redisplay (if not running a child)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	return (0);
}

void	me_handle_signals(int signo)
{
	muh_number = signo;
}

void	signal_init(struct sigaction *handler)
{
	sigset_t	mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGQUIT);//one of these literally handles sigquit bruh
	handler[0].sa_mask = mask;
	handler[1].sa_mask = mask;

	handler[0].sa_handler = me_handle_signals;
	handler[1].sa_handler = me_handle_signals;
}

void	env_print(t_env *env)
{
	t_shnode	*iter;

	iter = env->env;
	ft_putstr("\n\nenv list\n\n");
	while (iter)
	{
		ft_printf("%s = ", iter->name);
		if (iter->str)
			ft_putstr(iter->str);
		else
			ft_putstr("NULL");
		ft_putchar('\n');
		iter = iter->next;
	}
	iter = env->export;
	ft_putstr("\n\nexport list\n\n");
	while (iter)
	{
		ft_printf("%s = ", iter->name);
		if (iter->str)
			ft_putstr(iter->str);
		else
			ft_putstr("NULL");
		ft_putchar('\n');
		iter = iter->next;
	}
	ft_putstr("\n\n");
}

int main(int c, char **v, char **e)
{
	char				*buf;
	struct sigaction	handler[2];//not needed
	struct sigaction	old[2];
	t_env				env;

	(void) c;
	(void) v;
	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	ft_memset(&handler[0], 0, sizeof(struct sigaction));
	ft_memset(&handler[1], 0, sizeof(struct sigaction));
	signal_init(handler);
	rl_signal_event_hook = rl_handle_signals;
	sigaction(SIGINT, &handler[0], &old[0]);
	sigaction(SIGQUIT, &handler[1], &old[1]);
	muh_number = 0;
	env_init(&env, e);
//	env_print(&env);
	while (1)
	{
		buf = readline("I am a shell% ");
//		if (muh_number)
//			free(buf);
//		else
		shell_print(buf, &env);
		if (/*!muh_number &&*/ !buf)
		{
			ft_putstr("exiting now\n");
			rl_clear_history();
			exit(0);
		}
		muh_number = 0;
	}
	(void) buf;
}
