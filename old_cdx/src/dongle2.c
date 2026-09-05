

#include "codexion.h"

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

bool	dongle_try_take(t_dongle *d, t_coder *c)
{
	int	idx;

	pthread_mutex_unlock(&c->cond_mutex);
	pthread_mutex_lock(&d->mutex);
	idx = heap_find(d, c->id);
	if (idx == 0
		&& dongle_ready(d, c->sim->dongle_cooldown)
		&& !sim_is_stopped(c->sim))
	{
		heap_remove(d, 0);
		d->in_use = true;
		d->release_time = 0;
		pthread_mutex_unlock(&d->mutex);
		pthread_mutex_lock(&c->cond_mutex);
		return (true);
	}
	pthread_mutex_unlock(&d->mutex);
	pthread_mutex_lock(&c->cond_mutex);
	return (false);
}
