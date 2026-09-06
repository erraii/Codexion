#include "codexion.h"
#include <time.h>

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