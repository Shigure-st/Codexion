#include <pthread.h>
#include "codexion.h"

void wakeup_all_thread(t_SharedContext *ctx, int coder)
{
  int i;

  i = 0;
  while(i < coder)
  {
      pthread_cond_broadcast(&(ctx->coders[i].cond));
      i++;
  }
}

int run_simulation(t_SharedContext *ctx)
{
  int i;
  int j;
  t_Coder *coder;

  i = 0;
  if (pthread_create(&ctx->mon->th, NULL, check_burnout, ctx->mon) != 0)
    return -1;
  while(i < ctx->coder)
  {
    coder = &ctx->coders[i];
    if (pthread_create(&coder->th, NULL, simulate, coder) != 0)
    {
      ctx->stop_flag = true;
      wakeup_all_thread(ctx, i);
      break;
    }
    i++;
  }
  j = 0;
  while(j < i)
    pthread_join(ctx->coders[j++].th, NULL);
  pthread_join(ctx->mon->th, NULL);
  return 0;
}
