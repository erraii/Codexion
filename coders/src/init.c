#include "codexion.h"
#include <stdlib.h>

static void	init_dongles(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		simulation->dongles[i].id = i;
		simulation->dongles[i].owner_id = 0;
		simulation->dongles[i].cooldown_until = 0;
		i++;
	}
}

static void	init_coders(t_simulation *simulation)
{
	int	count;
	int	i;

	count = simulation->config.number_of_coders;
	i = 0;
	while (i < count)
	{
		simulation->coders[i].id = i + 1;
		simulation->coders[i].left_dongle = i;
		simulation->coders[i].right_dongle = (i + 1) % count;
		simulation->coders[i].compiles_done = 0;
		simulation->coders[i].last_compile_start = 0;
		simulation->coders[i].simulation = simulation;
		i++;
	}
}

int	init_simulation(t_simulation *simulation, t_config *config)
{
	simulation->config = *config;
	simulation->start_time = 0;
    simulation->started = 0;
    simulation->stopped = 0;
    simulation->request_sequence = 0;
	simulation->coders = NULL;
	simulation->dongles = malloc(sizeof(t_dongle)
			* config->number_of_coders);
	if (!simulation->dongles)
		return (1);
	simulation->coders = malloc(sizeof(t_coder)
			* config->number_of_coders);
	if (!simulation->coders)
	{
		free(simulation->dongles);
		simulation->dongles = NULL;
		return (1);
	}
	init_dongles(simulation);
	init_coders(simulation);
    if (init_sync(simulation))
    {
        free(simulation->coders);
        free(simulation->dongles);
        simulation->coders = NULL;
        simulation->dongles = NULL;
        return (1);
    }
	return (0);
}

void	destroy_simulation(t_simulation *simulation)
{
    destroy_sync(simulation);
	free(simulation->coders);
	free(simulation->dongles);
	simulation->coders = NULL;
	simulation->dongles = NULL;
}