#include "codexion.h"

void	lock_dongles(t_dongle *left, t_dongle *right)
{
	if (left == right)
		pthread_mutex_lock(&left->mutex);
	else if (left->id < right->id)
	{
		pthread_mutex_lock(&left->mutex);
		pthread_mutex_lock(&right->mutex);
	}
	else
	{
		pthread_mutex_lock(&right->mutex);
		pthread_mutex_lock(&left->mutex);
	}
}

void	unlock_dongles(t_dongle *left, t_dongle *right)
{
	if (left == right)
		pthread_mutex_unlock(&left->mutex);
	else if (left->id < right->id)
	{
		pthread_mutex_unlock(&right->mutex);
		pthread_mutex_unlock(&left->mutex);
	}
	else
	{
		pthread_mutex_unlock(&left->mutex);
		pthread_mutex_unlock(&right->mutex);
	}
}

static int	queues_have_space(t_dongle *left, t_dongle *right)
{
	if (left->queue.size >= DONGLE_QUEUE_CAPACITY)
		return (0);
	if (left != right && right->queue.size >= DONGLE_QUEUE_CAPACITY)
		return (0);
	return (1);
}

void	create_request(t_coder *coder, t_request *request)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->state_mutex);
	request->coder_id = coder->id;
	request->sequence = simulation->request_sequence;
	simulation->request_sequence++;
	request->deadline = coder->last_compile_start
		+ simulation->config.time_to_burnout;
	pthread_mutex_unlock(&simulation->state_mutex);
}

int	enqueue_request(t_coder *coder, t_request request)
{
	t_simulation	*simulation;
	t_dongle		*left;
	t_dongle		*right;
	int				result;

	simulation = coder->simulation;
	left = &simulation->dongles[coder->left_dongle];
	right = &simulation->dongles[coder->right_dongle];
	pthread_mutex_lock(&simulation->wait_mutex);
	lock_dongles(left, right);
	result = 1;
	if (queues_have_space(left, right))
	{
		heap_push(&left->queue, request, simulation->config.scheduler);
		if (left != right)
			heap_push(&right->queue, request,
				simulation->config.scheduler);
		result = 0;
	}
	unlock_dongles(left, right);
	if (result == 0)
		pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
	return (result);
}