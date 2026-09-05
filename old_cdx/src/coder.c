
#include "codexion.h"

static void	pick_dongle_order(t_coder *c,
				t_dongle **first,
				t_dongle **second)
{
	if (c->left->id < c->right->id)
	{
		*first = c->left;
		*second = c->right;
	}
	else
	{
		*first = c->right;
		*second = c->left;
	}
}

static bool	take_two_dongles(t_coder *c)
{
	t_dongle	*first;
	t_dongle	*second;

	pick_dongle_order(c, &first, &second);
	dongle_request(first, c);
	if (sim_is_stopped(c->sim))
	{
		dongle_release(first, c->sim);
		return (false);
	}
	log_event(c->sim, c->id, "has taken a dongle");
	dongle_request(second, c);
	if (sim_is_stopped(c->sim))
	{
		dongle_release(second, c->sim);
		dongle_release(first, c->sim);
		return (false);
	}
	log_event(c->sim, c->id, "has taken a dongle");
	return (true);
}

static bool	take_dongles(t_coder *c)
{
	if (sim_is_stopped(c->sim))
		return (false);
	if (c->sim->n == 1)
	{
		dongle_request(c->left, c);
		if (sim_is_stopped(c->sim))
		{
			dongle_release(c->left, c->sim);
			return (false);
		}
		log_event(c->sim, c->id, "has taken a dongle");
		return (true);
	}
	return (take_two_dongles(c));
}

static bool	coder_cycle(t_coder *c, t_sim *sim)
{
	if (!take_dongles(c) || sim_is_stopped(sim))
	{
		if (sim_is_stopped(sim) && c->sim->n != 1)
			put_dongles(c);
		return (false);
	}
	c->last_compile_start = get_time_ms();
	log_event(sim, c->id, "is compiling");
	msleep(sim->time_to_compile);
	c->compile_count++;
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
	if (sim->n > 1 && c->id % 2 == 0)
		usleep((useconds_t)(sim->time_to_compile / 2 * 1000LL));
	while (!sim_is_stopped(sim))
	{
		if (!coder_cycle(c, sim))
			break ;
	}
	return (NULL);
}
