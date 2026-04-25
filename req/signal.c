#include "h_minishell.h"

int	rl_handle_signals(void)//sigaction flag interrupt, rl handler	//does nl + redisplay (if not running a child)
{
	//else if (muh_number == SIGQUIT)
	//	ft_putstr("^\\");
	if (muh_number == SIGINT)
	{
		ft_putstr("^C");
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	muh_number = 0;
	return (0);
}

void	sighands(int signo)
{
	muh_number = signo;
}

void	signal_init(void)
{
	sigset_t			mask;
	struct sigaction	hands;

	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	rl_signal_event_hook = rl_handle_signals;
	if (ft_err(-(sigemptyset(&mask) || sigaddset(&mask, SIGTERM)
		|| sigaddset(&mask, SIGQUIT)), "signal mask error"))
		return ;
	ft_memset(&hands, 0, sizeof(struct sigaction));
	hands = (struct sigaction){.sa_mask = mask, .sa_handler = sighands};//, .sa_flags = SA_RESTART;//exclude restart flag
	ft_err(sigaction(SIGINT, &hands, NULL), "SIGINT setup error");
	ft_err(sigaction(SIGQUIT, &hands, NULL), "SIGQUIT setup error");
	muh_number = 0;
}
