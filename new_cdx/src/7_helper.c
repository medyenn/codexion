/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   7_helper.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:47 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 18:30:04 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	put_dongles(t_coder *c)
{
	dongle_release(c->sim, c);
}

bool	coder_wait_grant(t_sim *sim, t_coder *c)
{
	bool	got;

	pthread_mutex_lock(&c->cond_mutex);
	while (!c->granted && !sim_is_stopped(sim))
	{
		dongle_wait(c);
		if (c->granted || sim_is_stopped(sim))
			break ;
		pthread_mutex_unlock(&c->cond_mutex);
		pthread_mutex_lock(&sim->arb_mutex);
		dispatch(sim);
		pthread_mutex_unlock(&sim->arb_mutex);
		pthread_mutex_lock(&c->cond_mutex);
	}
	got = c->granted;
	pthread_mutex_unlock(&c->cond_mutex);
	return (got);
}

void	dongle_wait(t_coder *c)
{
	struct timespec	ts;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000L + 500000L;
	if (ts.tv_nsec >= 1000000000L)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000L;
	}
	pthread_cond_timedwait(&c->cond, &c->cond_mutex, &ts);
}

int	heap_find(t_sim *sim, int coder_id)
{
	int	i;

	i = 0;
	while (i < sim->heap_size)
	{
		if (sim->heap[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}

void	heap_remove(t_sim *sim, int idx)
{
	sim->heap_size--;
	if (idx < sim->heap_size)
	{
		sim->heap[idx] = sim->heap[sim->heap_size];
		heap_sift_up(sim->heap, idx);
		heap_sift_down(sim->heap, sim->heap_size, idx);
	}
}
