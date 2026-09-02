#include "codexion.h"

static void	swap_requests(t_request *first, t_request *second)
{
	t_request	temporary;

	temporary = *first;
	*first = *second;
	*second = temporary;
}

int	request_has_priority(const t_request *first,
		const t_request *second, t_scheduler scheduler)
{
	if (scheduler == POLICY_FIFO)
		return (first->sequence < second->sequence);
	if (first->deadline != second->deadline)
		return (first->deadline < second->deadline);
	return (first->sequence < second->sequence);
}

int	heap_push(t_heap *heap, t_request request, t_scheduler scheduler)
{
	int	index;
	int	parent;

	if (heap->size >= DONGLE_QUEUE_CAPACITY)
		return (1);
	index = heap->size;
	heap->items[index] = request;
	heap->size++;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_has_priority(&heap->items[index],
				&heap->items[parent], scheduler))
			break ;
		swap_requests(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
	return (0);
}
