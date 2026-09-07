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

static long long	get_cooldown_deadline(t_dongle *left,
		t_dongle *right, t_request request)
{
	long long	deadline;

	if (left == right)
		return (0);
	if (!request_is_top(&left->queue, request)
		|| !request_is_top(&right->queue, request))
		return (0);
	if (left->owner_id != 0 || right->owner_id != 0)
		return (0);
	deadline = left->cooldown_until;
	if (right->cooldown_until > deadline)
		deadline = right->cooldown_until;
	return (deadline);
}

static int	claim_dongles(t_coder *coder, t_request request,
		t_dongle *left, t_dongle *right)
{
	t_simulation	*simulation;
	long long		now;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->state_mutex);
	now = get_time_ms();
	if (simulation->stopped || now >= request.deadline)
	{
		pthread_mutex_unlock(&simulation->state_mutex);
		return (0);
	}
	left->owner_id = coder->id;
	right->owner_id = coder->id;
	heap_pop(&left->queue);
	heap_pop(&right->queue);
	coder->last_compile_start = now;
	pthread_mutex_unlock(&simulation->state_mutex);
	return (1);
}

int	try_acquire_dongles_locked(t_coder *coder, t_request request,
		long long *cooldown)
{
	t_simulation	*simulation;
	t_dongle		*left;
	t_dongle		*right;
	int				acquired;

	simulation = coder->simulation;
	left = &simulation->dongles[coder->left_dongle];
	right = &simulation->dongles[coder->right_dongle];
	lock_dongles(left, right);
	acquired = request_can_acquire(coder, request);
	if (acquired)
		acquired = claim_dongles(coder, request, left, right);
	if (!acquired)
		*cooldown = get_cooldown_deadline(left, right, request);
	unlock_dongles(left, right);
	return (acquired);
}