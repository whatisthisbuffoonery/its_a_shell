/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dthoo <dthoo@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 07:48:54 by dthoo             #+#    #+#             */
/*   Updated: 2026/06/14 07:48:54 by dthoo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "h_minishell.h"

int	rl_heredoc(void)
{
	if (g_signo == SIGQUIT)
		g_signo = 0;
	else if (g_signo == SIGINT)
	{
		rl_done = 1;
		ft_putstr("^C");
	}
	return (0);
}

int	rl_handle_signals(void)
{
	if (g_signo == SIGINT)
	{
		ft_putstr("^C");
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	g_signo = 0;
	return (0);
}

//use different handler for builtin//VETO//UNVETO
//sigquit is allowed to influence every command if it shows up in the parent
void	sighands(int n)
{
	if (g_signo != SIGINT)
		g_signo = n;
}

//, .sa_flags = SA_RESTART;//exclude restart flag
void	signal_init(void)
{
	sigset_t			mask;
	struct sigaction	hands;

	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	g_signo = 0;
	rl_signal_event_hook = rl_handle_signals;
	if (ft_err(-(sigemptyset(&mask) || sigaddset(&mask, SIGTERM)
				|| sigaddset(&mask, SIGQUIT)), "signal mask error"))
		return ;
	ft_memset(&hands, 0, sizeof(struct sigaction));
	hands = (struct sigaction){.sa_mask = mask, .sa_handler = sighands};
	ft_err(sigaction(SIGINT, &hands, NULL), "SIGINT setup error");
	ft_err(sigaction(SIGQUIT, &hands, NULL), "SIGQUIT setup error");
}
