#include "codexion.h"

int	wait_for_start(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->wait_mutex);
	while (!simulation->started
		&& !simulation_is_stopped(simulation))
	{
		pthread_cond_wait(&simulation->wait_condition,
			&simulation->wait_mutex);
	}
	stopped = simulation_is_stopped(simulation);
	pthread_mutex_unlock(&simulation->wait_mutex);
	return (stopped);
}

void	begin_simulation(t_simulation *simulation)
{
	int	i;

	pthread_mutex_lock(&simulation->wait_mutex);
	pthread_mutex_lock(&simulation->state_mutex);
	simulation->start_time = get_time_ms();
	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		simulation->coders[i].last_compile_start
			= simulation->start_time;
		i++;
	}
	pthread_mutex_unlock(&simulation->state_mutex);
	simulation->started = 1;
	pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
}

int	create_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		if (pthread_create(&simulation->coders[i].thread, NULL,
				coder_routine, &simulation->coders[i]) != 0)
		{
			stop_simulation(simulation);
			while (i > 0)
			{
				i--;
				pthread_join(simulation->coders[i].thread, NULL);
			}
			return (1);
		}
		i++;
	}
	return (0);
}

void	join_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
}

int	run_simulation(t_simulation *simulation)
{
	if (create_coder_threads(simulation) != 0)
		return (1);
	if (pthread_create(&simulation->monitor_thread, NULL,
			monitor_routine, simulation) != 0)
	{
		stop_simulation(simulation);
		join_coder_threads(simulation);
		return (1);
	}
	begin_simulation(simulation);
	pthread_join(simulation->monitor_thread, NULL);
	join_coder_threads(simulation);
	return (0);
}