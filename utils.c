#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "codexion.h"

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

void	output_log(t_shared_context *ctx, int id, const char *message)
{
	long long	elapsed;

	pthread_mutex_lock(&ctx->log_lock);
	if (strcmp(message, "burned out") == 0)
		set_stop_flag(ctx);
	else if (is_stopped(ctx))
	{
		pthread_mutex_unlock(&ctx->log_lock);
		return ;
	}
	elapsed = get_time_in_ms() - ctx->start_time_ms;
	printf("%lld %d %s\n", elapsed, id, message);
	pthread_mutex_unlock(&ctx->log_lock);
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
