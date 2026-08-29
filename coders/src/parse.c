#include "codexion.h"
#include <limits.h>
#include <string.h>

static int	parse_number(const char *str, int *number)
{
	int	i;
	int	result;
	int	digit;

	if (!str || !str[0])
		return (1);
	i = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (1);
	result = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		digit = str[i] - '0';
		if (result > (INT_MAX - digit) / 10)
			return (1);
		result = result * 10 + digit;
		i++;
	}
	*number = result;
	return (0);
}

static int	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (strcmp(str, "fifo") == 0)
		*scheduler = POLICY_FIFO;
	else if (strcmp(str, "edf") == 0)
		*scheduler = POLICY_EDF;
	else
		return (1);
	return (0);
}

int	parse_arguments(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		return (1);
	if (parse_number(argv[1], &config->number_of_coders)
		|| parse_number(argv[2], &config->time_to_burnout)
		|| parse_number(argv[3], &config->time_to_compile)
		|| parse_number(argv[4], &config->time_to_debug)
		|| parse_number(argv[5], &config->time_to_refactor)
		|| parse_number(argv[6], &config->number_of_compiles_required)
		|| parse_number(argv[7], &config->dongle_cooldown)
		|| parse_scheduler(argv[8], &config->scheduler))
		return (1);
	if (config->number_of_coders == 0)
		return (1);
	return (0);
}