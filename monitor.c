#include "codexion.h"
#include <pthread.h>
#include <stdlib.h>


int alloc_monitor(t_SharedContext *shared_ctx)
{
  shared_ctx->monitor = malloc(sizeof(t_Monitor));
  if (shared_ctx->monitor == NULL)
    return (cleanup_context(shared_ctx));
  shared_ctx->monitor->shared_ctx = shared_ctx;
  if (pthread_mutex_init(&shared_ctx->monitor->burnout_mutex, NULL) != 0)
  {
    free(shared_ctx->monitor);
    shared_ctx->monitor = NULL;
    return (cleanup_context(shared_ctx));
  }
  return 0;
}
