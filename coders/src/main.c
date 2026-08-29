#include "codexion.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_config	config;

	if (parse_arguments(argc, argv, &config))
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
    printf("%lld\n", get_time_ms());
	return (0);
}