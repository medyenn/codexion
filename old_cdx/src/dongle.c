

#include "codexion.h"

int	dongle_init(t_dongle *d, int id, t_scheduler sched)
{
	d->id = id;
	d->in_use = false;
	d->release_time = 0;
	d->heap_size = 0;
	d->heap_cap = 8;
	d->scheduler = sched;
	d->heap = (t_request *)malloc((size_t)d->heap_cap * sizeof(t_request));
	if (!d->heap)
		return (-1);
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
	{
		free(d->heap);
		d->heap = NULL;
		return (-1);
	}
	return (0);
}

void	dongle_destroy(t_dongle *d)
{
	pthread_mutex_destroy(&d->mutex);
	free(d->heap);
	d->heap = NULL;
}

void	dongle_request(t_dongle *d, t_coder *c)
{
	t_request	req;

	req.priority = get_time_ms();
	if (d->scheduler == POLICY_EDF)
		req.priority = c->last_compile_start + c->sim->time_to_burnout;
	req.coder_id = c->id;
	req.cond = &c->cond;
	pthread_mutex_lock(&d->mutex);
	heap_push(d, req);
	pthread_mutex_lock(&c->cond_mutex);
	pthread_mutex_unlock(&d->mutex);
	while (1)
	{
		if (dongle_try_take(d, c))
			break ;
		if (sim_is_stopped(c->sim))
			break ;
		dongle_wait(c);
	}
	pthread_mutex_unlock(&c->cond_mutex);
}

void	dongle_release(t_dongle *d, t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&d->mutex);
	d->in_use = false;
	d->release_time = get_time_ms();
	i = 0;
	while (i < d->heap_size)
	{
		pthread_mutex_lock(&sim->coders[d->heap[i].coder_id - 1].cond_mutex);
		pthread_cond_signal(d->heap[i].cond);
		pthread_mutex_unlock(
			&sim->coders[d->heap[i].coder_id - 1].cond_mutex);
		i++;
	}
	pthread_mutex_unlock(&d->mutex);
}
