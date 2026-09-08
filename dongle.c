#include "codexion.h"
#include <pthread.h>
#include <stdlib.h>

int alloc_dongle_array(t_SharedContext *shared_ctx)
{
  int i;
  int j;

  i = 0;
  shared_ctx->dongles = malloc(sizeof(t_Dongle) * shared_ctx->coder);
  if(shared_ctx->dongles == NULL)
    return (cleanup_context(shared_ctx));
  while(i < shared_ctx->coder)
  {
    // shared_ctx->dongles[i].i = i;
    shared_ctx->dongles[i].available = true;
    shared_ctx->dongles[i].cooldown_end_time = 0;
    shared_ctx->dongles[i].ts.tv_nsec = 0;
    shared_ctx->dongles[i].ts.tv_sec = 0;
    shared_ctx->dongles[i].wait_coders = NULL;
	  if(pthread_mutex_init(&shared_ctx->dongles[i].dongle_lock, NULL) != 0)
    {
      j = 0;
      while(j < i)
        pthread_mutex_destroy(&shared_ctx->dongles[j++].dongle_lock);
      free(shared_ctx->dongles);
      shared_ctx->dongles = NULL;
      return (cleanup_context(shared_ctx));
    }
    i++;
  }
  return 0;
}
