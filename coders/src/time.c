#include "codexion.h"
#include <sys/time.h>
#include <stddef.h>

long long	get_time_ms(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		return (-1);
	return ((long long)time.tv_sec * 1000 + time.tv_usec / 1000);
}