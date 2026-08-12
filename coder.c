#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

void init_coder_struct(t_SharedContext *shared_ctx, int i)
{
  shared_ctx->coders[i].number = i + 1;
  shared_ctx->coders[i].shared_ctx = shared_ctx;
  shared_ctx->coders[i].is_compile = false;
  shared_ctx->coders[i].wait_cond = false;
  shared_ctx->coders[i].left_dongle = &shared_ctx->dongles[i];
  shared_ctx->coders[i].boss = shared_ctx->boss;
  if (i == shared_ctx->coder - 1)
    shared_ctx->coders[i].right_dongle = &shared_ctx->dongles[0];
  else
    shared_ctx->coders[i].right_dongle = &shared_ctx->dongles[i + 1];
}

int alloc_coder_array(t_SharedContext *shared_ctx)
{
  int i;
  int j;

  i = 0;
  shared_ctx->coders = malloc(sizeof(t_Coder) * shared_ctx->coder);
  if (shared_ctx->coders == NULL)
    return (cleanup_context(shared_ctx));
  while (i < shared_ctx->coder)
  {
	  if(pthread_cond_init(&shared_ctx->coders[i].check_compile_cond, NULL) != 0)
    {
      j = 0;
      while(j < i)
      {
        pthread_cond_destroy(&shared_ctx->coders[j].check_compile_cond);
        j++;
      }
      free(shared_ctx->coders);
      shared_ctx->coders = NULL;
      return (cleanup_context(shared_ctx));
    }
    init_coder_struct(shared_ctx, i);
    i++;
  }
  return 0;
}
