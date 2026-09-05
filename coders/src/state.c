#include "codexion.h"

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