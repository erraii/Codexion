#include "codexion.h"
#include <stdio.h>
#include <unistd.h>

static t_coder	*inspect_coders(t_simulation *simulation, int *all_done)
{
	t_coder		*burned;
	long long	now;
	int			i;

	burned = NULL;
	*all_done = 1;
	now = get_time_ms();
	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		if (simulation->coders[i].compiles_done
			< simulation->config.number_of_compiles_required)
			*all_done = 0;
		if (burned == NULL && now
			- simulation->coders[i].last_compile_start
			>= simulation->config.time_to_burnout)
			burned = &simulation->coders[i];
		i++;
	}
	return (burned);
}

static void	stop_for_burnout(t_simulation *simulation, t_coder *coder)
{
	long long	timestamp;

	simulation->stopped = 1;
	timestamp = get_time_ms() - simulation->start_time;
	printf("%lld %d %s\n", timestamp, coder->id, MSG_BURNOUT);
}

static int	check_simulation(t_simulation *simulation)
{
	t_coder	*burned;
	int		all_done;
	int		stopped;

	pthread_mutex_lock(&simulation->log_mutex);
	pthread_mutex_lock(&simulation->state_mutex);
	stopped = simulation->stopped;
	if (!stopped)
	{
		burned = inspect_coders(simulation, &all_done);
		if (all_done)
			simulation->stopped = 1;
		else if (burned != NULL)
			stop_for_burnout(simulation, burned);
		stopped = simulation->stopped;
	}
	pthread_mutex_unlock(&simulation->state_mutex);
	pthread_mutex_unlock(&simulation->log_mutex);
	return (stopped);
}

static void	wake_waiting_threads(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->wait_mutex);
	pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
}

void	*monitor_routine(void *argument)
{
	t_simulation	*simulation;

	simulation = (t_simulation *)argument;
	if (wait_for_start(simulation))
		return (NULL);
	while (!check_simulation(simulation))
		usleep(500);
	wake_waiting_threads(simulation);
	return (NULL);
}