#include "codexion.h"

int	request_has_priority(const t_request *first,
		const t_request *second, t_scheduler scheduler)
{
	if (scheduler == POLICY_FIFO)
		return (first->sequence < second->sequence);
	if (first->deadline != second->deadline)
		return (first->deadline < second->deadline);
	return (first->sequence < second->sequence);
}