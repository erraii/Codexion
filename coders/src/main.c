#include "codexion.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	simulation;

	if (parse_arguments(argc, argv, &config) != 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (init_simulation(&simulation, &config) != 0)
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	if (run_simulation(&simulation) != 0)
	{
		fprintf(stderr, "Error: thread creation failed\n");
		destroy_simulation(&simulation);
		return (1);
	}
	destroy_simulation(&simulation);
	return (0);
}