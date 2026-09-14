#include <stdbool.h>
#include <pthread.h>
#include "codexion.h"

void	order_by_address(t_Coder *coder)
{
	if (coder->r_dongle < coder->l_dongle)
	{
		coder->first = coder->r_dongle;
		coder->second = coder->l_dongle;
	}
	else
	{
		coder->first = coder->l_dongle;
		coder->second = coder->r_dongle;
	}
}

void	take_dongles(t_Coder *coder)
{
	pthread_mutex_lock(&(coder->first->lock));
	pthread_mutex_lock(&(coder->second->lock));
	coder->first->free = false;
	coder->second->free = false;
	pthread_mutex_unlock(&(coder->first->lock));
	pthread_mutex_unlock(&(coder->second->lock));
	output_log(coder->ctx, coder->id, "has taken a dongle");
	output_log(coder->ctx, coder->id, "has taken a dongle");
}

void	release_dongles(t_Coder *coder)
{
	pthread_mutex_lock(&(coder->first->lock));
	pthread_mutex_lock(&(coder->second->lock));
	coder->r_dongle->t_end = get_time_in_ms() + coder->ctx->cooldown;
	coder->l_dongle->t_end = get_time_in_ms() + coder->ctx->cooldown;
	(coder->r_dongle->free) = true;
	(coder->l_dongle->free) = true;
	if (coder->r_dongle->cond != NULL)
		pthread_cond_broadcast(coder->r_dongle->cond);
	if (coder->l_dongle->cond != NULL)
		pthread_cond_broadcast(coder->l_dongle->cond);
	pthread_mutex_unlock(&(coder->first->lock));
	pthread_mutex_unlock(&(coder->second->lock));
}

void	wait_for_dongles(t_Coder *coder)
{
	struct timespec	wakeup;
	bool			acquire;

	acquire = false;
	if (!is_empty_and_free(coder->r_dongle))
		heap_push(coder->r_dongle, coder);
	if (!is_empty_and_free(coder->l_dongle))
		heap_push(coder->l_dongle, coder);
	pthread_mutex_lock(&(coder->lock));
	while (!acquire && !is_stopped(coder->ctx))
	{
		wakeup = wakeup_time(coder);
		pthread_cond_timedwait(&(coder->cond), &(coder->lock), &wakeup);
		acquire = try_to_acquire(coder);
	}
	pthread_mutex_unlock(&(coder->lock));
}
