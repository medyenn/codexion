

#include "codexion.h"

static bool	all_compiled(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		if (sim->coders[i].compile_count < sim->compiles_required)
			return (false);
		i++;
	}
	return (true);
}

static int	check_burnout(t_sim *sim)
{
	long long	now;
	long long	elapsed;
	int			i;

	now = get_time_ms();
	i = 0;
	while (i < sim->n)
	{
		elapsed = now - sim->coders[i].last_compile_start;
		if (elapsed >= sim->time_to_burnout)
			return (i + 1);
		i++;
	}
	return (0);
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
	int	d;

	signal_coders(sim);
	d = 0;
	while (d < sim->n)
	{
		pthread_mutex_lock(&sim->dongles[d].mutex);
		pthread_mutex_unlock(&sim->dongles[d].mutex);
		d++;
	}
	signal_coders(sim);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		burned;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		usleep(500);
		if (sim_is_stopped(sim))
			break ;
		if (sim->compiles_required > 0 && all_compiled(sim))
		{
			sim_stop(sim);
			wake_all_coders(sim);
			break ;
		}
		burned = check_burnout(sim);
		if (burned != 0)
		{
			sim_stop(sim);
			log_burnout(sim, burned);
			wake_all_coders(sim);
			break ;
		}
	}
	return (NULL);
}
