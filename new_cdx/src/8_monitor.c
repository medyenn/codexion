/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   8_monitor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:25:59 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 19:34:28 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	all_compiled(t_sim *sim)
{
	int		i;
	int		count;

	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->coders[i].cond_mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].cond_mutex);
		if (count < sim->compiles_required)
			return (false);
		i++;
	}
	return (true);
}

/*
** One pass over every coder: reports who burned out (0 if nobody did)
** and, either way, how long the monitor may safely sleep before it
** needs to look again (the time left until the closest deadline,
** capped at 10ms so a completed simulation is still noticed promptly).
*/
static int	scan_coders(t_sim *sim, long long *wake_us)
{
	long long	now;
	long long	start;
	long long	remain;
	int			i;
	int			burned;

	now = get_time_ms();
	*wake_us = 10 * 1000LL;
	burned = 0;
	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->coders[i].cond_mutex);
		start = sim->coders[i].last_compile_start;
		pthread_mutex_unlock(&sim->coders[i].cond_mutex);
		remain = start + sim->time_to_burnout - now;
		if (remain <= 0 && burned == 0)
			burned = i + 1;
		if (remain * 1000LL < *wake_us)
			*wake_us = remain * 1000LL;
		i++;
	}
	if (*wake_us < 200)
		*wake_us = 200;
	return (burned);
}

static void	signal_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->coders[i].cond_mutex);
		pthread_cond_broadcast(&sim->coders[i].cond);
		pthread_mutex_unlock(&sim->coders[i].cond_mutex);
		i++;
	}
}

static void	wake_all_coders(t_sim *sim)
{
	signal_coders(sim);
	pthread_mutex_lock(&sim->arb_mutex);
	pthread_mutex_unlock(&sim->arb_mutex);
	signal_coders(sim);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	int			burned;
	long long	wake_us;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		if (sim->compiles_required > 0 && all_compiled(sim))
		{
			sim_stop(sim);
			wake_all_coders(sim);
			break ;
		}
		burned = scan_coders(sim, &wake_us);
		if (burned != 0)
		{
			sim_stop(sim);
			log_burnout(sim, burned);
			wake_all_coders(sim);
			break ;
		}
		usleep((useconds_t)wake_us);
	}
	return (NULL);
}
