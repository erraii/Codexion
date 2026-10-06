/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecakiray <ecakiray@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 11:27:41 by ecakiray          #+#    #+#             */
/*   Updated: 2026/10/06 15:38:42 by ecakiray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stddef.h>
#include <sys/time.h>
#include <time.h>

long long	get_time_ms(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		return (-1);
	return ((long long)time.tv_sec * 1000 + time.tv_usec / 1000);
}

static void	set_timeout(struct timespec *timeout, long long end_time)
{
	timeout->tv_sec = end_time / 1000;
	timeout->tv_nsec = (end_time % 1000) * 1000000;
}

int	sleep_ms(t_simulation *simulation, int duration)
{
	struct timespec	timeout;
	long long		end_time;
	int				stopped;

	end_time = get_time_ms() + duration;
	set_timeout(&timeout, end_time);
	pthread_mutex_lock(&simulation->wait_mutex);
	while (!simulation_is_stopped(simulation)
		&& get_time_ms() < end_time)
	{
		pthread_cond_timedwait(&simulation->wait_condition,
			&simulation->wait_mutex, &timeout);
	}
	stopped = simulation_is_stopped(simulation);
	pthread_mutex_unlock(&simulation->wait_mutex);
	return (stopped);
}
