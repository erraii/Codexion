/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecakiray <ecakiray@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 11:27:44 by ecakiray          #+#    #+#             */
/*   Updated: 2026/10/06 15:51:30 by ecakiray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <time.h>

static void	wait_for_event(t_simulation *simulation, long long cooldown)
{
	struct timespec	timeout;

	if (cooldown > get_time_ms())
	{
		timeout.tv_sec = cooldown / 1000;
		timeout.tv_nsec = (cooldown % 1000) * 1000000;
		pthread_cond_timedwait(&simulation->wait_condition,
			&simulation->wait_mutex, &timeout);
	}
	else if (cooldown == 0)
		pthread_cond_wait(&simulation->wait_condition,
			&simulation->wait_mutex);
}

int	wait_for_dongles(t_coder *coder, t_request request)
{
	t_simulation	*simulation;
	long long		cooldown;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->wait_mutex);
	while (!simulation_is_stopped(simulation))
	{
		cooldown = 0;
		if (try_acquire_dongles_locked(coder, request, &cooldown))
		{
			pthread_mutex_unlock(&simulation->wait_mutex);
			return (0);
		}
		wait_for_event(simulation, cooldown);
	}
	pthread_mutex_unlock(&simulation->wait_mutex);
	return (1);
}

void	release_dongles(t_coder *coder)
{
	t_simulation	*simulation;
	t_dongle		*left;
	t_dongle		*right;
	long long		cooldown_end;

	simulation = coder->simulation;
	left = &simulation->dongles[coder->left_dongle];
	right = &simulation->dongles[coder->right_dongle];
	pthread_mutex_lock(&simulation->wait_mutex);
	lock_dongles(left, right);
	cooldown_end = get_time_ms()
		+ simulation->config.dongle_cooldown;
	if (left->owner_id == coder->id)
	{
		left->owner_id = 0;
		left->cooldown_until = cooldown_end;
	}
	if (right != left && right->owner_id == coder->id)
	{
		right->owner_id = 0;
		right->cooldown_until = cooldown_end;
	}
	unlock_dongles(left, right);
	pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
}
