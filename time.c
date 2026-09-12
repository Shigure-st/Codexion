#include "codexion.h"

struct timespec ms_to_timespec(long long ms)
{
  struct timespec  ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;

  return ts;
}

long long get_cooldown_time(t_Dongle *right, t_Dongle *left)
{
  if (right->t_end >= left->t_end)
    return right->t_end;
  else
    return left->t_end;
}

struct timespec wakeup_time(t_Coder *coder)
{
  long long now;
  long long target;

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

long long  get_time_in_ms(void)
{
  struct timeval  tv;
  long long time;

  gettimeofday(&tv, NULL);
  time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  return time;
}
