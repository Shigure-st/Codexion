/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:46 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:46 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include "codexion.h"

int	alloc_monitor(t_shared_context *ctx)
{
	ctx->mon = malloc(sizeof(t_monitor));
	if (ctx->mon == NULL)
		return (cleanup_context(ctx));
	ctx->mon->ctx = ctx;
	if (pthread_cond_init(&ctx->mon->cond, NULL) != 0)
	{
		free(ctx->mon);
		ctx->mon = NULL;
		return (cleanup_context(ctx));
	}
	if (pthread_mutex_init(&ctx->mon->lock, NULL) != 0)
	{
		pthread_cond_destroy(&ctx->mon->cond);
		free(ctx->mon);
		ctx->mon = NULL;
		return (cleanup_context(ctx));
	}
	return (0);
}

void	output_burnout_log(t_shared_context *ctx, int id, long long usec)
{
	long long			elapsed;

	pthread_mutex_lock(&ctx->log_lock);
	set_stop_flag(ctx);
	elapsed = usec_to_ms(usec - ctx->start_time_usec);
	printf("%lld %d burned out\n", elapsed, id);
	pthread_mutex_unlock(&ctx->log_lock);
}

static bool	check_single_coder_burnout(t_coder *coder, t_monitor *mon)
{
	long long			last_t;
	long long			now_t;
	long long			elapsed;
	t_shared_context	*ctx;

	if (coder->done)
		return (false);
	last_t = get_last_compile_time(coder);
	if (last_t == 0)
		return (false);
	ctx = mon->ctx;
	if (last_t + (ctx->burnout * 1000) < mon->w_time)
		mon->w_time = last_t + (ctx->burnout * 1000);
	now_t = get_time_in_usec();
	elapsed = now_t - last_t;
	if (elapsed >= ctx->burnout * 1000)
	{
		output_burnout_log(ctx, coder->id, now_t);
		wakeup_all_thread(ctx, ctx->coder);
		return (true);
	}
	return (false);
}

void	monitor_sleep(t_monitor *mon, long long target_usec)
{
	struct timespec	wakeup;

	wakeup = usec_to_timespec(target_usec);
	pthread_mutex_lock(&mon->lock);
	pthread_cond_timedwait(&mon->cond, &mon->lock, &wakeup);
	pthread_mutex_unlock(&mon->lock);
}

void	*check_burnout(void *arg)
{
	int			i;
	t_monitor	*mon;
	long long	target_usec;

	mon = arg;
	while (!is_stopped(mon->ctx))
	{
		i = 0;
		mon->w_time = LLONG_MAX;
		while (i < mon->ctx->coder)
		{
			if (check_single_coder_burnout(&mon->ctx->coders[i], mon))
				return (NULL);
			i++;
		}
		target_usec = mon->w_time;
		if (target_usec == LLONG_MAX)
			target_usec = get_time_in_usec() + (mon->ctx->burnout * 1000 / 2);
		monitor_sleep(mon, target_usec);
	}
	return (NULL);
}
