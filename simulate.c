#include <pthread.h>
#include "codexion.h"

void wakeup_all_thread(t_SharedContext *shared_ctx, int coder)
{
  int i;

  i = 0;
  while(i < coder)
  {
      pthread_cond_broadcast(&(shared_ctx->coders[i].check_compile_cond));
      i++;
  }
  pthread_cond_broadcast(&(shared_ctx->boss->shared_ctx->cond));
}

int run_simulation(t_SharedContext *shared_ctx)
{
  int i;
  int j;

  i = 0;
  if (pthread_create(&shared_ctx->boss->t_Boss, NULL, receive_from_coder, shared_ctx->boss) != 0)
    return -1;
  while(i < shared_ctx->coder)
  {
    if (pthread_create(&shared_ctx->coders[i].t_Coder, NULL, simulate, &shared_ctx->coders[i]) != 0)
    {
      shared_ctx->stop_flag = true;
      wakeup_all_thread(shared_ctx, i);
      break;
    }
    i++;
  }
  j = 0;
  while(j < i)
    pthread_join(shared_ctx->coders[j++].t_Coder, NULL);
  pthread_join(shared_ctx->boss->t_Boss, NULL);
  return 0;
}
