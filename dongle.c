/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:42 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:42 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include "codexion.h"

bool	is_empty_and_free(t_dongle *dongle)
{
	bool	empty_and_free;

	pthread_mutex_lock(&(dongle->lock));
	empty_and_free = (dongle->free
			&& dongle->waiters->size == 0
			&& get_time_in_ms() >= dongle->t_end);
	pthread_mutex_unlock(&(dongle->lock));
	return (empty_and_free);
}

bool	is_ready_to_take(t_dongle *dongle, t_coder *coder)
{
	bool	heap_top;
	bool	heap_empty;
	bool	cooldown_over;
	bool	ready;

	heap_empty = (dongle->waiters->size == 0);
	heap_top = (!heap_empty && dongle->waiters->data[0].coder == coder);
	cooldown_over = (get_time_in_ms() >= dongle->t_end);
	ready = (dongle->free && cooldown_over && (heap_top || heap_empty));
	return (ready);
}

bool	try_to_acquire(t_coder *coder)
{
	bool	ok;

	pthread_mutex_lock(&(coder->first->lock));
	pthread_mutex_lock(&(coder->second->lock));
	ok = (is_ready_to_take(coder->r_dongle, coder)
			&& is_ready_to_take(coder->l_dongle, coder));
	if (ok)
	{
		heap_pop(coder->r_dongle);
		heap_pop(coder->l_dongle);
		coder->r_dongle->free = false;
		coder->l_dongle->free = false;
		output_log(coder->ctx, coder->id, "has taken a dongle");
		output_log(coder->ctx, coder->id, "has taken a dongle");
	}
	pthread_mutex_unlock(&(coder->first->lock));
	pthread_mutex_unlock(&(coder->second->lock));
	return (ok);
}

static void	init_value(t_dongle *dongle)
{
	dongle->free = true;
	dongle->t_end = 0;
	dongle->ts.tv_nsec = 0;
	dongle->ts.tv_sec = 0;
	dongle->waiters = NULL;
}

int	alloc_dongle_array(t_shared_context *ctx)
{
	int	i;
	int	j;

	i = 0;
	ctx->dongles = malloc(sizeof(t_dongle) * ctx->coder);
	if (ctx->dongles == NULL)
		return (cleanup_context(ctx));
	while (i < ctx->coder)
	{
		init_value(&ctx->dongles[i]);
		if (pthread_mutex_init(&ctx->dongles[i].lock, NULL) != 0)
		{
			j = 0;
			while (j < i)
				pthread_mutex_destroy(&ctx->dongles[j++].lock);
			free(ctx->dongles);
			ctx->dongles = NULL;
			return (cleanup_context(ctx));
		}
		i++;
	}
	return (0);
}
