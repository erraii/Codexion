/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecakiray <ecakiray@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 11:27:30 by ecakiray          #+#    #+#             */
/*   Updated: 2026/10/06 15:46:10 by ecakiray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

int	simulation_is_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->state_mutex);
	stopped = simulation->stopped;
	pthread_mutex_unlock(&simulation->state_mutex);
	return (stopped);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->state_mutex);
	simulation->stopped = 1;
	pthread_mutex_unlock(&simulation->state_mutex);
	pthread_mutex_lock(&simulation->wait_mutex);
	pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
}

int	coder_has_finished(t_coder *coder)
{
	t_simulation	*simulation;
	int				finished;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->state_mutex);
	finished = coder->compiles_done
		>= simulation->config.number_of_compiles_required;
	pthread_mutex_unlock(&simulation->state_mutex);
	return (finished);
}

void	log_state(t_coder *coder, const char *message)
{
	t_simulation	*simulation;
	long long		timestamp;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->log_mutex);
	if (!simulation_is_stopped(simulation))
	{
		timestamp = get_time_ms() - simulation->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, message);
	}
	pthread_mutex_unlock(&simulation->log_mutex);
}
