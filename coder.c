#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

#include <stdio.h>


static void init_coder_struct(t_SharedContext *ctx, int i)
{
  ctx->coders[i].id = i + 1;
  ctx->coders[i].ctx = ctx;
  ctx->coders[i].is_comp = false;
  ctx->coders[i].wait = false;
  ctx->coders[i].l_dongle = &ctx->dongles[i];
  // ctx->coders[i].boss = ctx->boss;
  ctx->coders[i].mon = ctx->mon;
  ctx->coders[i].done = false;
  ctx->coders[i].t_last = 0;
  if (i == ctx->coder - 1)
    ctx->coders[i].r_dongle = &ctx->dongles[0];
  else
    ctx->coders[i].r_dongle = &ctx->dongles[i + 1];
}

int init_coder_mutex(t_SharedContext *ctx)
{
  int i;
  int j;
  int k;

  i = 0;
  k = 0;
  while (i < ctx->coder)
  {
    if (pthread_mutex_init(&ctx->coders[i].lock, NULL) != 0)
    {
      j = 0;
      while(j < i)
        pthread_mutex_destroy(&ctx->coders[j++].lock);
      while(k < ctx->coder)
        pthread_cond_destroy(&ctx->coders[k++].cond);
      free(ctx->coders);
      ctx->coders = NULL;
      return (cleanup_context(ctx));
    }
    i++;
  }
  return 0;
}

int alloc_coder_array(t_SharedContext *ctx)
{
  int i;
  int j;

  i = 0;
  ctx->coders = malloc(sizeof(t_Coder) * ctx->coder);
  if (ctx->coders == NULL)
    return (cleanup_context(ctx));
  while (i < ctx->coder)
  {
	  if(pthread_cond_init(&ctx->coders[i].cond, NULL) != 0)
    {
      j = 0;
      while(j < i)
        pthread_cond_destroy(&ctx->coders[j++].cond);
      free(ctx->coders);
      ctx->coders = NULL;
      return (cleanup_context(ctx));
    }
    init_coder_struct(ctx, i);
    i++;
  }
  return 0;
}

int coder_cycle(t_Coder *coder)
{
  acquire_dongles(coder);
  if (coder->ctx->stop_flag || is_compile(coder) == -1)
    return 1;
  if (coder->ctx->stop_flag || is_debug(coder) == -1)
    return 1;
  if (coder->ctx->stop_flag || is_refactor(coder) == -1)
    return 1;
  return 0;

}

void	*simulate(void* arg)
{
	struct s_Coder	*coder;
  int i;

	coder = arg;
  coder->t_last = get_time_in_ms();
  if (coder->r_dongle == coder->l_dongle)
  {
    pthread_mutex_lock(&(coder->lock));
    while (!coder->ctx->stop_flag)
      pthread_cond_wait(&(coder->cond), &(coder->lock));
    pthread_mutex_unlock(&(coder->lock));
    return NULL;
  }
  i = 0;
  while(i < coder->ctx->required)
  {
    if (coder_cycle(coder))
        break;
    printf("Coder:%d compile number:%d\n", coder->id, i + 1);
    i++;
  }
  printf("compile complete\n");
  coder->done = true;
  if (check_complete(coder->ctx))
    wakeup_all_thread(coder->ctx, coder->ctx->coder);
  //   pthread_cond_broadcast(&(coder->shared_ctx->queue->not_empty));
  return NULL;
}
