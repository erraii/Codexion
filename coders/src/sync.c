#include "codexion.h"
#include <stddef.h>

static void	destroy_global_sync(t_simulation *simulation)
{
	pthread_cond_destroy(&simulation->wait_condition);
	pthread_mutex_destroy(&simulation->wait_mutex);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->state_mutex);
}

static int	init_global_sync(t_simulation *simulation)
{
	if (pthread_mutex_init(&simulation->state_mutex, NULL))
		return (1);
	if (pthread_mutex_init(&simulation->log_mutex, NULL))
	{
		pthread_mutex_destroy(&simulation->state_mutex);
		return (1);
	}
	if (pthread_mutex_init(&simulation->wait_mutex, NULL))
	{
		pthread_mutex_destroy(&simulation->log_mutex);
		pthread_mutex_destroy(&simulation->state_mutex);
		return (1);
	}
	if (pthread_cond_init(&simulation->wait_condition, NULL))
	{
		pthread_mutex_destroy(&simulation->wait_mutex);
		pthread_mutex_destroy(&simulation->log_mutex);
		pthread_mutex_destroy(&simulation->state_mutex);
		return (1);
	}
	return (0);
}

static int	init_dongle_mutexes(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		if (pthread_mutex_init(&simulation->dongles[i].mutex, NULL))
		{
			while (i > 0)
			{
				i--;
				pthread_mutex_destroy(&simulation->dongles[i].mutex);
			}
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_sync(t_simulation *simulation)
{
	if (init_global_sync(simulation))
		return (1);
	if (init_dongle_mutexes(simulation))
	{
		destroy_global_sync(simulation);
		return (1);
	}
	return (0);
}

void	destroy_sync(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		i++;
	}
	destroy_global_sync(simulation);
}