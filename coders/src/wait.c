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