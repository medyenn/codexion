/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_dongle.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:25:16 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 18:29:45 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	dongle_init(t_dongle *d, int id)
{
	d->id = id;
	d->in_use = false;
	d->release_time = 0;
}

bool	dongle_ready(t_dongle *d, long long cooldown_ms)
{
	long long	now;

	if (d->in_use)
		return (false);
	if (d->release_time == 0)
		return (true);
	now = get_time_ms();
	return (now >= d->release_time + cooldown_ms);
}

/*
** Try to grant every request that CAN be granted right now, always
** considering the highest-priority (earliest ticket / soonest
** deadline) request first. A request only ever waits behind a
** higher-priority one when they truly conflict over the same
** dongle - an unrelated, currently-free pair is never left idle
** just because someone else, elsewhere on the ring, is stuck.
** A coder needing the same dongle twice (n == 1) can never satisfy
** "two DISTINCT dongles" -> permanently refused here: guaranteed
** burnout, with no special-casing needed anywhere else.
** Must be called with sim->arb_mutex already held.
*/
void	dispatch(t_sim *sim)
{
	t_request	req;
	t_coder		*c;
	int			pending;

	pending = 0;
	while (sim->heap_size > 0)
	{
		req = sim->heap[0];
		heap_remove(sim, 0);
		c = &sim->coders[req.coder_id - 1];
		if (c->left != c->right
			&& dongle_ready(c->left, sim->dongle_cooldown)
			&& dongle_ready(c->right, sim->dongle_cooldown))
		{
			c->left->in_use = true;
			c->right->in_use = true;
			pthread_mutex_lock(&c->cond_mutex);
			c->granted = true;
			pthread_cond_signal(&c->cond);
			pthread_mutex_unlock(&c->cond_mutex);
		}
		else
			sim->pending[pending++] = req;
	}
	while (pending > 0)
		heap_push(sim, sim->pending[--pending]);
}

void	dongle_request(t_sim *sim, t_coder *c)
{
	t_request	req;
	int			idx;

	req.coder_id = c->id;
	req.priority = c->ticket;
	if (sim->scheduler == POLICY_EDF)
		req.priority = c->last_compile_start + sim->time_to_burnout;
	pthread_mutex_lock(&c->cond_mutex);
	c->granted = false;
	pthread_mutex_unlock(&c->cond_mutex);
	pthread_mutex_lock(&sim->arb_mutex);
	heap_push(sim, req);
	dispatch(sim);
	pthread_mutex_unlock(&sim->arb_mutex);
	if (!coder_wait_grant(sim, c))
	{
		pthread_mutex_lock(&sim->arb_mutex);
		idx = heap_find(sim, c->id);
		if (idx != -1)
			heap_remove(sim, idx);
		pthread_mutex_unlock(&sim->arb_mutex);
	}
}

void	dongle_release(t_sim *sim, t_coder *c)
{
	pthread_mutex_lock(&sim->arb_mutex);
	c->left->in_use = false;
	c->left->release_time = get_time_ms();
	if (c->right != c->left)
	{
		c->right->in_use = false;
		c->right->release_time = get_time_ms();
	}
	dispatch(sim);
	pthread_mutex_unlock(&sim->arb_mutex);
}
