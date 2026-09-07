#include "codexion.h"

static void	increment_compile_count(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->state_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&simulation->state_mutex);
}

static int	compile_code(t_coder *coder)
{
	t_request	request;

	create_request(coder, &request);
	if (enqueue_request(coder, request) != 0)
		return (1);
	if (wait_for_dongles(coder, request) != 0)
		return (1);
	log_state(coder, MSG_DONGLE);
	log_state(coder, MSG_DONGLE);
	log_state(coder, MSG_COMPILE);
	if (sleep_ms(coder->simulation,
			coder->simulation->config.time_to_compile))
	{
		release_dongles(coder);
		return (1);
	}
	release_dongles(coder);
	increment_compile_count(coder);
	return (0);
}

static int	debug_and_refactor(t_coder *coder)
{
	log_state(coder, MSG_DEBUG);
	if (sleep_ms(coder->simulation,
			coder->simulation->config.time_to_debug))
		return (1);
	log_state(coder, MSG_REFACTOR);
	if (sleep_ms(coder->simulation,
			coder->simulation->config.time_to_refactor))
		return (1);
	return (0);
}

void	*coder_routine(void *argument)
{
	t_coder	*coder;

	coder = (t_coder *)argument;
	while (!simulation_is_stopped(coder->simulation))
	{
		if (compile_code(coder) != 0)
			break ;
		if (debug_and_refactor(coder) != 0)
			break ;
	}
	return (NULL);
}