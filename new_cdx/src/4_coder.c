/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_coder.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:30 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 19:30:38 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** One combined, atomic request for both of this coder's dongles.
** The ticket is captured once, right here, so the whole attempt is
** ranked as a single arrival, not two. dongle_request only returns
** once the arbiter has granted both dongles together, or the
** simulation has stopped.
*/
static bool	take_dongles(t_coder *c)
{
	if (sim_is_stopped(c->sim))
		return (false);
	c->ticket = get_time_ms();
	dongle_request(c->sim, c);
	if (sim_is_stopped(c->sim))
		return (false);
	log_event(c->sim, c->id, "has taken a dongle");
	log_event(c->sim, c->id, "has taken a dongle");
	return (true);
}

static bool	coder_cycle(t_coder *c, t_sim *sim)
{
	pthread_mutex_lock(&c->cond_mutex);
	if (c->compile_count >= sim->compiles_required)
	{
		pthread_mutex_unlock(&c->cond_mutex);
		return (false);
	}
	pthread_mutex_unlock(&c->cond_mutex);
	if (!take_dongles(c))
		return (false);
	pthread_mutex_lock(&c->cond_mutex);
	c->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&c->cond_mutex);
	log_event(sim, c->id, "is compiling");
	msleep(sim->time_to_compile);
	pthread_mutex_lock(&c->cond_mutex);
	c->compile_count++;
	pthread_mutex_unlock(&c->cond_mutex);
	put_dongles(c);
	if (sim_is_stopped(sim))
		return (false);
	log_event(sim, c->id, "is debugging");
	msleep(sim->time_to_debug);
	if (sim_is_stopped(sim))
		return (false);
	log_event(sim, c->id, "is refactoring");
	msleep(sim->time_to_refactor);
	return (true);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_sim	*sim;

	c = (t_coder *)arg;
	sim = c->sim;
	while (!sim_is_stopped(sim))
	{
		if (!coder_cycle(c, sim))
			break ;
	}
	return (NULL);
}
