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
    return -1;
  while(i < shared_ctx->coder)
  {
    shared_ctx->dongles[i].i = i;
    shared_ctx->dongles[i].available = true;
	  if(pthread_mutex_init(&shared_ctx->dongles[i].dongle_lock, NULL) != 0)
    {
      j = 0;
      while(j < i)
      {
        pthread_mutex_destroy(&shared_ctx->dongles[j].dongle_lock);
        j++;
      }
      free(shared_ctx->dongles);
      return -1;
    }
    i++;
  }
  return 0;
}
