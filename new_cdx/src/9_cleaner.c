/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:08 by mennih            #+#    #+#             */
/*   Updated: 2026/09/02 14:27:29 by mennih           ###   ########.fr       */
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
