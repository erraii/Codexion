#include "codexion.h"

int	request_is_top(t_heap *heap, t_request request)
{
	t_request	*top;

	top = heap_peek(heap);
	if (top == NULL)
		return (0);
	if (top->coder_id != request.coder_id)
		return (0);
	if (top->sequence != request.sequence)
		return (0);
	return (1);
}

int	request_can_acquire(t_coder *coder, t_request request)
{
	t_simulation	*simulation;
	t_dongle		*left;
	t_dongle		*right;
	long long		now;

	simulation = coder->simulation;
	left = &simulation->dongles[coder->left_dongle];
	right = &simulation->dongles[coder->right_dongle];
	if (left == right)
		return (0);
	if (!request_is_top(&left->queue, request)
		|| !request_is_top(&right->queue, request))
		return (0);
	if (left->owner_id != 0 || right->owner_id != 0)
		return (0);
	now = get_time_ms();
	if (left->cooldown_until > now || right->cooldown_until > now)
		return (0);
	return (1);
}