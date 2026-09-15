/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_action.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:37 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:37 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdbool.h>
#include "codexion.h"

int	is_compile(t_coder *coder)
{
	if (update_last_compile_time(coder))
		return (-1);
	output_log(coder->ctx, coder->id, "is compiling");
	set_coder_sleep(coder, coder->ctx->compile);
	pthread_mutex_lock(&(coder->lock));
	while (!is_expired(coder) && !is_stopped(coder->ctx))
		pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
	pthread_mutex_unlock(&(coder->lock));
	release_dongles(coder);
	if (is_stopped(coder->ctx))
		return (-1);
	return (0);
}

int	is_debug(t_coder *coder)
{
	output_log(coder->ctx, coder->id, "is debugging");
	set_coder_sleep(coder, coder->ctx->debug);
	pthread_mutex_lock(&(coder->lock));
	while (!is_expired(coder) && !is_stopped(coder->ctx))
		pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
	pthread_mutex_unlock(&(coder->lock));
	if (is_stopped(coder->ctx))
		return (-1);
	return (0);
}

int	is_refactor(t_coder *coder)
{
	output_log(coder->ctx, coder->id, "is refactoring");
	set_coder_sleep(coder, coder->ctx->refactor);
	pthread_mutex_lock(&(coder->lock));
	while (!is_expired(coder) && !is_stopped(coder->ctx))
		pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
	pthread_mutex_unlock(&(coder->lock));
	if (is_stopped(coder->ctx))
		return (-1);
	return (0);
}

void	acquire_dongles(t_coder *coder)
{
	if (is_empty_and_free(coder->r_dongle)
		&& is_empty_and_free(coder->l_dongle))
		take_dongles(coder);
	else
		wait_for_dongles(coder);
}

bool	handle_single_coder(t_coder *coder)
{
	if (coder->r_dongle != coder->l_dongle)
		return (false);
	pthread_mutex_lock(&(coder->lock));
	while (!is_stopped(coder->ctx))
		pthread_cond_wait(&(coder->cond), &(coder->lock));
	pthread_mutex_unlock(&(coder->lock));
	return (true);
}
