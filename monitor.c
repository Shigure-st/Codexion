#include "codexion.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>


bool is_stopped(t_SharedContext *ctx)
{
  bool stop;

  pthread_mutex_lock(&ctx->lock);
  stop = ctx->stop_flag;
  pthread_mutex_unlock(&ctx->lock);
  return stop;
}

void set_stop_flag(t_SharedContext *ctx)
{
  pthread_mutex_lock(&ctx->lock);
  ctx->stop_flag = true;
  pthread_mutex_unlock(&ctx->lock);
}

int alloc_monitor(t_SharedContext *ctx)
{
  ctx->mon = malloc(sizeof(t_Monitor));
  if (ctx->mon == NULL)
    return (cleanup_context(ctx));
  ctx->mon->ctx = ctx;
	if(pthread_cond_init(&ctx->mon->cond, NULL) != 0)
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
  return 0;
}

void update_last_compile_time(t_Coder *coder)
{
  pthread_mutex_lock(&(coder->lock));
  coder->t_last = get_time_in_ms();
  pthread_mutex_unlock(&(coder->lock));
}

long long get_last_compile_time(t_Coder *coder)
{
  long long t;

  pthread_mutex_lock(&(coder->lock));
  t = coder->t_last;
  pthread_mutex_unlock(&(coder->lock));
  return t;
}

static bool check_single_coder_burnout(t_Coder *coder, t_Monitor *mon)
{
  long long last_t;
  long long now_t;
  t_SharedContext *ctx;

  if (coder->done)
    return false;
  last_t = get_last_compile_time(coder);
  if (last_t == 0)
    return false;
  ctx = mon->ctx;
  if (last_t + ctx-> burnout + 2 < mon->w_time)
    mon->w_time = last_t + ctx-> burnout + 2;
  now_t = get_time_in_ms();
  printf("last_compile_time:%lld\n", last_t);
  if ((now_t - last_t) > ctx->burnout)
  {
    printf("経過時間:%lld\n", (now_t - last_t));
    set_stop_flag(ctx);
    wakeup_all_thread(ctx, ctx->coder);
    printf("coder:%d のプログラムは燃え尽きた\n", coder->id);
    return true;
  }
  return false;
}

void monitor_sleep(t_Monitor *mon, long long target_ms)
{
  struct timespec wakeup;

  wakeup = ms_to_timespec(target_ms);
  pthread_mutex_lock(&mon->lock);
  pthread_cond_timedwait(&mon->cond, &mon->lock, &wakeup);
  pthread_mutex_unlock(&mon->lock);
}

void  *check_burnout(void* arg)
{
  int i;
  t_Monitor  *mon;
  long long target_ms;

  mon = arg;
  while (!is_stopped(mon->ctx))
  {
    i = 0;
    mon->w_time = LLONG_MAX;
    while(i < mon->ctx->coder)
    {
      if (check_single_coder_burnout(&mon->ctx->coders[i], mon))
        return NULL;
      i++;
    }
    target_ms = mon->w_time;
    if (target_ms == LLONG_MAX)
      target_ms = get_time_in_ms() + (mon->ctx->burnout / 2);
    monitor_sleep(mon, target_ms);
    // usleep(1000);
  }
  return NULL;
}
