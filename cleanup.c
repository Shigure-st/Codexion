#include "codexion.h"
#include <stdlib.h>

static void cleanup_dongles(t_SharedContext *ctx)
{
  int i;

  i = 0;
  if(ctx->dongles != NULL)
  {
    while(i < ctx->coder)
    {
      pthread_mutex_destroy(&ctx->dongles[i].lock);
      i++;
    }
    free(ctx->dongles);
    ctx->dongles = NULL;
  }
}

static void cleanup_monitor(t_SharedContext *ctx)
{
  if (ctx->mon != NULL)
  {
    pthread_mutex_destroy(&ctx->mon->lock);
    pthread_cond_destroy(&ctx->mon->cond);
    free(ctx->mon);
    ctx->mon = NULL;
  }
}

static void cleanup_coders(t_SharedContext *ctx)
{
  int i;

  i = 0;
  if (ctx->coders != NULL)
  {
    while(i < ctx->coder)
    {
      pthread_cond_destroy(&ctx->coders[i].cond);
      pthread_mutex_destroy(&ctx->coders[i].lock);
      i++;
    }
    free(ctx->coders);
    ctx->coders = NULL;
  }
}
static void cleanup_heapqueue(t_SharedContext *ctx)
{
  int i;

  if (ctx == NULL || ctx->dongles == NULL)
    return;
  i = 0;
  while (i < ctx->coder)
  {
    free_dongle_heap(&ctx->dongles[i]);
    i++;
  }
}

int  cleanup_context(t_SharedContext *ctx)
{
  if (ctx == NULL)
    return -1;
  if (ctx->is_cond)
    pthread_cond_destroy(&ctx->cond);
  if (ctx->is_lock)
    pthread_mutex_destroy(&ctx->lock);
  if (ctx->is_log_lock)
    pthread_mutex_destroy(&ctx->log_lock);
  cleanup_heapqueue(ctx);
  cleanup_dongles(ctx);
  cleanup_coders(ctx);
  cleanup_monitor(ctx);
  return -1;
}
