#include "codexion.h"

void	release_dongles(t_coder *coder)
{
	t_simulation	*simulation;
	t_dongle		*left;
	t_dongle		*right;
	long long		cooldown_end;

	simulation = coder->simulation;
	left = &simulation->dongles[coder->left_dongle];
	right = &simulation->dongles[coder->right_dongle];
	pthread_mutex_lock(&simulation->wait_mutex);
	lock_dongles(left, right);
	cooldown_end = get_time_ms()
		+ simulation->config.dongle_cooldown;
	if (left->owner_id == coder->id)
	{
		left->owner_id = 0;
		left->cooldown_until = cooldown_end;
	}
	if (right != left && right->owner_id == coder->id)
	{
		right->owner_id = 0;
		right->cooldown_until = cooldown_end;
	}
	unlock_dongles(left, right);
	pthread_cond_broadcast(&simulation->wait_condition);
	pthread_mutex_unlock(&simulation->wait_mutex);
}