#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
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

static bool	check_single_coder_burnout(t_coder *coder, t_monitor *mon)
{
	long long			last_t;
	long long			now_t;
	t_shared_context	*ctx;

	if (coder->done)
		return (false);
	last_t = get_last_compile_time(coder);
	if (last_t == 0)
		return (false);
	ctx = mon->ctx;
	if (last_t + ctx->burnout < mon->w_time)
		mon->w_time = last_t + ctx->burnout;
	now_t = get_time_in_ms();
	if ((now_t - last_t) >= ctx->burnout)
	{
		output_log(ctx, coder->id, "burned out");
		wakeup_all_thread(ctx, ctx->coder);
		return (true);
	}
	return (false);
}

void	monitor_sleep(t_monitor *mon, long long target_ms)
{
	struct timespec	wakeup;

	wakeup = ms_to_timespec(target_ms);
	pthread_mutex_lock(&mon->lock);
	pthread_cond_timedwait(&mon->cond, &mon->lock, &wakeup);
	pthread_mutex_unlock(&mon->lock);
}

void	*check_burnout(void *arg)
{
	int			i;
	t_monitor	*mon;
	long long	target_ms;

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
		target_ms = mon->w_time;
		if (target_ms == LLONG_MAX)
			target_ms = get_time_in_ms() + (mon->ctx->burnout / 2);
		monitor_sleep(mon, target_ms);
	}
	return (NULL);
}
