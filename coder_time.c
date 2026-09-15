#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include "codexion.h"

struct timespec	wakeup_time(t_coder *coder)
{
	long long	now;
	long long	target;

	now = get_time_in_ms();
	target = now + coder->ctx->compile + coder->ctx->cooldown;
	if (coder->r_dongle->free && coder->r_dongle->t_end > now)
		target = coder->r_dongle->t_end;
	if (coder->l_dongle->free
		&& coder->l_dongle->t_end > now
		&& coder->l_dongle->t_end < target)
		target = coder->l_dongle->t_end;
	return (ms_to_timespec(target));
}

bool	update_last_compile_time(t_coder *coder)
{
	long long	t;

	t = get_time_in_ms();
	pthread_mutex_lock(&(coder->lock));
	if (coder->t_last != 0 && t >= coder->t_last + coder->ctx->burnout)
	{
		pthread_mutex_unlock(&(coder->lock));
		return (true);
	}
	coder->t_last = t;
	pthread_mutex_unlock(&(coder->lock));
	return (false);
}

long long	get_last_compile_time(t_coder *coder)
{
	long long	t;

	pthread_mutex_lock(&(coder->lock));
	t = coder->t_last;
	pthread_mutex_unlock(&(coder->lock));
	return (t);
}

void	set_coder_sleep(t_coder *coder, int wait_ms)
{
	long	total_usec;
	long	remainder_usec;

	gettimeofday(&coder->tv, NULL);
	total_usec = coder->tv.tv_usec + (wait_ms * 1000);
	coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
	remainder_usec = total_usec % 1000000;
	coder->ts.tv_nsec = remainder_usec * 1000;
}
