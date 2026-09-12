#include "codexion.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int alloc_monitor(t_SharedContext *ctx)
{
  ctx->mon = malloc(sizeof(t_Monitor));
  if (ctx->mon == NULL)
    return (cleanup_context(ctx));
  ctx->mon->ctx = ctx;
  if (pthread_mutex_init(&ctx->mon->lock, NULL) != 0)
  {
    free(ctx->mon);
    ctx->mon = NULL;
    return (cleanup_context(ctx));
  }
  return 0;
}

void  *check_burnout(void* arg)
{
  int i;
  long long now_time;
  long long last_compile_time;
  struct s_Monitor  *mon;

  mon = arg;
  while (!mon->ctx->stop_flag)
  {
    i = 0;
    while(i < mon->ctx->coder)
    {
      if (mon->ctx->coders[i].done)
      {
        i++;
        continue;
      }
      pthread_mutex_lock(&(mon->lock));
      last_compile_time = mon->ctx->coders[i].t_last;
      pthread_mutex_unlock(&(mon->lock));
      if (last_compile_time == 0)
      {
        i++;
        continue;
      }
      now_time = get_time_in_ms();
      if ((now_time - last_compile_time) > mon->ctx->burnout)
      {
        printf("経過時間:%lld\n", (now_time - last_compile_time));
        mon->ctx->stop_flag = true;
        wakeup_all_thread(mon->ctx, mon->ctx->coder);
        printf("coder:%d のプログラムは燃え尽きた\n", mon->ctx->coders[i].id);
        return NULL;
      }
      i++;
    }
    usleep(1000);
  }
  return NULL;
}
