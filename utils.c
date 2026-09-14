#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "codexion.h"

bool	is_stopped(t_SharedContext *ctx)
{
	bool	stop;

	pthread_mutex_lock(&ctx->lock);
	stop = ctx->stop_flag;
	pthread_mutex_unlock(&ctx->lock);
	return (stop);
}

void	set_stop_flag(t_SharedContext *ctx)
{
	pthread_mutex_lock(&ctx->lock);
	ctx->stop_flag = true;
	pthread_mutex_unlock(&ctx->lock);
}

void	output_log(t_SharedContext *ctx, int id, const char *message)
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
