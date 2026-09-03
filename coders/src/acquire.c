#include <time.h>
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

static int	try_acquire_locked(t_coder *coder, t_request request,
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
	{
		left->owner_id = coder->id;
		right->owner_id = coder->id;
		heap_pop(&left->queue);
		heap_pop(&right->queue);
	}
	else
		*cooldown = get_cooldown_deadline(left, right, request);
	unlock_dongles(left, right);
	return (acquired);
}

int	wait_for_dongles(t_coder *coder, t_request request)
{
	t_simulation		*simulation;
	struct timespec	timeout;
	long long			cooldown;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->wait_mutex);
	cooldown = 0;
	while (!try_acquire_locked(coder, request, &cooldown))
	{
		if (cooldown > get_time_ms())
		{
			timeout.tv_sec = cooldown / 1000;
			timeout.tv_nsec = (cooldown % 1000) * 1000000;
			pthread_cond_timedwait(&simulation->wait_condition,
				&simulation->wait_mutex, &timeout);
		}
		else if (cooldown == 0)
			pthread_cond_wait(&simulation->wait_condition,
				&simulation->wait_mutex);
	}
	pthread_mutex_unlock(&simulation->wait_mutex);
	return (0);
}