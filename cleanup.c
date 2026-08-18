#include "codexion.h"
#include <stdlib.h>

static void cleanup_dongles(t_SharedContext *shared_ctx)
{
  int i;

  i = 0;
  if(shared_ctx->dongles != NULL)
  {
    while(i < shared_ctx->coder)
    {
      pthread_mutex_destroy(&shared_ctx->dongles[i].dongle_lock);
      i++;
    }
    free(shared_ctx->dongles);
    shared_ctx->dongles = NULL;
  }
}

static void cleanup_boss(t_SharedContext *shared_ctx)
{
  if (shared_ctx->boss != NULL)
  {
    pthread_mutex_destroy(&shared_ctx->boss->request_mutex);
    free(shared_ctx->boss);
    shared_ctx->boss = NULL;
  }
}

static void cleanup_monitor(t_SharedContext *shared_ctx)
{
  if (shared_ctx->monitor != NULL)
  {
    pthread_mutex_destroy(&shared_ctx->monitor->burnout_mutex);
    free(shared_ctx->monitor);
    shared_ctx->monitor = NULL;
  }
}

static void cleanup_coders(t_SharedContext *shared_ctx)
{
  int i;

  i = 0;
  if (shared_ctx->coders != NULL)
  {
    while(i < shared_ctx->coder)
    {
      pthread_cond_destroy(&shared_ctx->coders[i].check_compile_cond);
      pthread_mutex_destroy(&shared_ctx->coders[i].local_mutex);
      i++;
    }
    free(shared_ctx->coders);
    shared_ctx->coders = NULL;
  }
}

static void cleanup_queue(t_SharedContext *shared_ctx)
{
  if (shared_ctx->queue != NULL)
  {
    pthread_cond_destroy(&shared_ctx->queue->not_empty);
    if (shared_ctx->queue->arr != NULL)
      free(shared_ctx->queue->arr);
    free(shared_ctx->queue);
    shared_ctx->queue = NULL;
  }
}

int  cleanup_context(t_SharedContext *shared_ctx)
{
  if (shared_ctx == NULL)
    return -1;
  pthread_cond_destroy(&shared_ctx->cond);
  cleanup_dongles(shared_ctx);
  cleanup_boss(shared_ctx);
  cleanup_coders(shared_ctx);
  cleanup_queue(shared_ctx);
  cleanup_monitor(shared_ctx);
  return -1;
}
