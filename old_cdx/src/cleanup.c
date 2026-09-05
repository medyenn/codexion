

#include "codexion.h"

static void	free_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		pthread_cond_destroy(&sim->coders[i].cond);
		pthread_mutex_destroy(&sim->coders[i].cond_mutex);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

static void	free_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
		dongle_destroy(&sim->dongles[i++]);
	free(sim->dongles);
	sim->dongles = NULL;
}

void	sim_cleanup(t_sim *sim)
{
	if (sim->threads)
	{
		free(sim->threads);
		sim->threads = NULL;
	}
	if (sim->coders)
		free_coders(sim);
	if (sim->dongles)
		free_dongles(sim);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}
