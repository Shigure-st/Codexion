#include <pthread.h>
#include <stdlib.h>
#include "codexion.h"

int alloc_boss(t_SharedContext *shared_ctx)
{
  shared_ctx->boss = malloc(sizeof(t_Boss));
  if (shared_ctx->boss == NULL)
    return (cleanup_context(shared_ctx));
  shared_ctx->boss->shared_ctx = shared_ctx;
  if (pthread_mutex_init(&shared_ctx->boss->request_mutex, NULL) != 0)
  {
    free(shared_ctx->boss);
    shared_ctx->boss = NULL;
    return (cleanup_context(shared_ctx));
  }
  return 0;
}
