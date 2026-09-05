
#include "codexion.h"

bool	sim_is_stopped(t_sim *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stopped;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stopped = true;
	pthread_mutex_unlock(&sim->stop_mutex);
}
