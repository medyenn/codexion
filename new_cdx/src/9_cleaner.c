/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   9_cleaner.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:08 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 20:07:31 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	free(sim->dongles);
	sim->dongles = NULL;
	free(sim->heap);
	sim->heap = NULL;
	free(sim->pending);
	sim->pending = NULL;
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
	pthread_mutex_destroy(&sim->arb_mutex);
}
