/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:46 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:46 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "codexion.h"

bool	is_done(t_coder *coder)
{
	bool	done;

	pthread_mutex_lock(&(coder->lock));
	done = coder->done;
	pthread_mutex_unlock(&(coder->lock));
	return (done);
}

bool	is_stopped(t_shared_context *ctx)
{
	bool	stop;

	pthread_mutex_lock(&ctx->lock);
	stop = ctx->stop_flag;
	pthread_mutex_unlock(&ctx->lock);
	return (stop);
}

void	set_stop_flag(t_shared_context *ctx)
{
	pthread_mutex_lock(&ctx->lock);
	ctx->stop_flag = true;
	pthread_mutex_unlock(&ctx->lock);
}

long long	output_log(t_shared_context *ctx, int id, char *message)
{
	long long	now;
	long long	elapsed;

	pthread_mutex_lock(&ctx->log_lock);
	if (is_stopped(ctx))
	{
		pthread_mutex_unlock(&ctx->log_lock);
		return (-1);
	}
	now = get_time_in_usec();
	elapsed = usec_to_ms(now - ctx->start_time_usec);
	printf("%lld %d %s\n", elapsed, id, message);
	pthread_mutex_unlock(&ctx->log_lock);
	return (now);
}

bool	is_expired(t_coder *coder)
{
	struct timeval	now_tv;
	long long		now_nsec;

	gettimeofday(&now_tv, NULL);
	now_nsec = now_tv.tv_usec * 1000;
	if (now_tv.tv_sec > coder->ts.tv_sec)
		return (true);
	if (now_tv.tv_sec < coder->ts.tv_sec)
		return (false);
	if (now_nsec >= coder->ts.tv_nsec)
		return (true);
	return (false);
}
