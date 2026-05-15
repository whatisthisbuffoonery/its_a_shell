#include "h_minishell.h"

int	rl_handle_signals(void)//sigaction flag interrupt, rl handler	//does nl + redisplay (if not running a child)
{
	//else if (signal == SIGQUIT)
	//	ft_putstr("^\\");
	if (signo == SIGINT)
	{
		ft_putstr("^C");
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	signo = 0;
	return (0);
}

//use different handler for builtin//VETO//UNVETO
//sigquit is allowed to influence every command if it shows up in the parent
void	sighands(int n)
{
	if (signo != SIGINT)	//final answer
		signo = n;
}
/*
void	sighands_builtin(int n)
{
	signo = n;
}

int	restore_signal_builtin(struct sigaction *old)
{
	int	i;

	i = ft_err(sigaction(SIGINT, old, NULL), "builtin signal restoration error");
	i += ft_err(sigaction(SIGQUIT, old, NULL), "builtin signal restoration error");
	if (signo == SIGQUIT)
		signo = 0;
	else
		ft_putstr("^C\n");//debatable
	return (i);
}

int	install_signal_builtin(struct sigaction *old)
{
	struct sigaction	new;

	ft_memset(old, 0, sizeof(struct sigaction));
	ft_memset(&new, 0, sizeof(struct sigaction));
	if (ft_err(sigaction(SIGINT, NULL, old), "mask retrieval failure"))
		return (1);
	new.sa_mask = old->sa_mask;
	new.sa_handler = sighands_builtin;
	if (ft_err(-(sigaction(SIGINT, new, NULL) || sigaction(SIGQUIT, new, NULL)), "builtin signal installation error"))//norm
	{
		restore_signal_builtin(old);
		return (1);
	}
	return (0);
}
*/
void	signal_init(void)
{
	sigset_t			mask;
	struct sigaction	hands;

	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	signo = 0;
	rl_signal_event_hook = rl_handle_signals;
	if (ft_err(-(sigemptyset(&mask) || sigaddset(&mask, SIGTERM)
		|| sigaddset(&mask, SIGQUIT)), "signal mask error"))
		return ;
	ft_memset(&hands, 0, sizeof(struct sigaction));
	hands = (struct sigaction){.sa_mask = mask, .sa_handler = sighands};//, .sa_flags = SA_RESTART;//exclude restart flag
	ft_err(sigaction(SIGINT, &hands, NULL), "SIGINT setup error");
	ft_err(sigaction(SIGQUIT, &hands, NULL), "SIGQUIT setup error");
}
