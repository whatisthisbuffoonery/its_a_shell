
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
	return (c && !isop(c) && !ft_isquote(c) && !ft_isspace(c) && !isbracket(c));
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
	ret->str = ft_substr(src, 0, i + (ft_isquote(c) != 0));
	ret->next = NULL;
	ret->env = NULL;
	ret->type = '\0';
	if (!ret->str)
		*cry = (err(-1, "cmd node str malloc"));
	ret->type = c;
	if (ret->str && !ft_strcmp(ret->str, "&"))//single & not required
		ret->type = '@';
	ret->end_space = ft_isspace(src[i + (src[i] && ft_isquote(c))]);//bool
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
			|| (ft_isquote(c) && src[i] && src[i] != c)				//quote, also operand
				|| (isbracket(c) && i < 1)))								//put brackets in their own node
		i ++;
	ret = cmd_node(src, i, c, cry);
	cmd_node_append(dst, ret);
	return (i + (ft_isquote(c) != 0));
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

void	print_cmd(t_cmd **cmd, int *last)
{
	t_cmd	*iter;

	iter = *cmd;
	while (iter)
	{
		ft_printf("[%s]\n", iter->str);
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

void	shell_print(char *buf)
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



int main(void)
{
	char				*buf;
	struct sigaction	handler[2];//not needed
	struct sigaction	old[2];

	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	ft_memset(&handler[0], 0, sizeof(struct sigaction));
	ft_memset(&handler[1], 0, sizeof(struct sigaction));
	signal_init(handler);
	rl_signal_event_hook = rl_handle_signals;
	sigaction(SIGINT, &handler[0], &old[0]);
	sigaction(SIGQUIT, &handler[1], &old[1]);
	muh_number = 0;
	while (1)
	{
		buf = readline("I am a shell% ");
//		if (muh_number)
//			free(buf);
//		else
		shell_print(buf);
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
