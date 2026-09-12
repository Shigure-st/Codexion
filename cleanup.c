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

// static void cleanup_boss(t_SharedContext *shared_ctx)
// {
//   if (shared_ctx->boss != NULL)
//   {
//     pthread_mutex_destroy(&shared_ctx->boss->request_mutex);
//     free(shared_ctx->boss);
//     shared_ctx->boss = NULL;
//   }
// }

static void cleanup_monitor(t_SharedContext *ctx)
{
  if (ctx->mon != NULL)
  {
    pthread_mutex_destroy(&ctx->mon->lock);
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

// static void cleanup_queue(t_SharedContext *shared_ctx)
// {
//   if (shared_ctx->queue != NULL)
//   {
//     pthread_cond_destroy(&shared_ctx->queue->not_empty);
//     if (shared_ctx->queue->arr != NULL)
//       free(shared_ctx->queue->arr);
//     free(shared_ctx->queue);
//     shared_ctx->queue = NULL;
//   }
// }

int  cleanup_context(t_SharedContext *ctx)
{
  if (ctx == NULL)
    return -1;
  pthread_cond_destroy(&ctx->cond);
  cleanup_dongles(ctx);
  cleanup_heapqueue(ctx);
  // cleanup_boss(ctx);
  cleanup_coders(ctx);
  // cleanup_queue(ctx);
  cleanup_monitor(ctx);
  return -1;
}
