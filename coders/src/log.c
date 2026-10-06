/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecakiray <ecakiray@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 11:27:04 by ecakiray          #+#    #+#             */
/*   Updated: 2026/10/06 11:27:05 by ecakiray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

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