/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   7_helper.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:47 by mennih            #+#    #+#             */
/*   Updated: 2026/09/05 13:45:55 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	put_dongles(t_coder *c)
{
	if (c->sim->n == 1)
	{
		dongle_release(c->left, c->sim);
		return ;
	}
	dongle_release(c->left, c->sim);
	dongle_release(c->right, c->sim);
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

void	dongle_destroy(t_dongle *d)
{
	pthread_mutex_destroy(&d->mutex);
	free(d->heap);
	d->heap = NULL;
}

int	heap_find(t_dongle *d, int coder_id)
{
	int	i;

	i = 0;
	while (i < d->heap_size)
	{
		if (d->heap[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}

void	heap_remove(t_dongle *d, int idx)
{
	d->heap_size--;
	if (idx < d->heap_size)
	{
		d->heap[idx] = d->heap[d->heap_size];
		heap_sift_up(d->heap, idx);
		heap_sift_down(d->heap, d->heap_size, idx);
	}
}
