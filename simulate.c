/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:46 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:46 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

void	wakeup_all_thread(t_shared_context *ctx, int coder)
{
	int	i;

	i = 0;
	while (i < coder)
	{
		pthread_mutex_lock(&(ctx->coders[i].lock));
		pthread_cond_broadcast(&(ctx->coders[i].cond));
		pthread_mutex_unlock(&(ctx->coders[i].lock));
		i++;
	}
}

static int	start_coders(t_shared_context *ctx)
{
	int		i;
	t_coder	*coder;

	i = 0;
	while (i < ctx->coder)
	{
		coder = &ctx->coders[i];
		if (pthread_create(&coder->th, NULL, simulate, coder) != 0)
		{
			set_stop_flag(ctx);
			wakeup_all_thread(ctx, i);
			break ;
		}
		i++;
	}
	return (i);
}

static void	join_all(t_shared_context *ctx, int started_count)
{
	int	j;

	j = 0;
	while (j < started_count)
		pthread_join(ctx->coders[j++].th, NULL);
	pthread_join(ctx->mon->th, NULL);
}

int	run_simulation(t_shared_context *ctx)
{
	int	started_count;

	ctx->start_time_ms = get_time_in_ms();
	if (pthread_create(&ctx->mon->th, NULL, check_burnout, ctx->mon) != 0)
		return (-1);
	started_count = start_coders(ctx);
	join_all(ctx, started_count);
	if (started_count < ctx->coder)
		return (-1);
	return (0);
}
