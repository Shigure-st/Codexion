#include <stdbool.h>
#include <pthread.h>
#include "codexion.h"

static int init_shared_context(t_Args *args, t_SharedContext *ctx)
{
	if(pthread_cond_init(&ctx->cond, NULL) != 0)
      return -1;
  ctx->coder = args->coder;
  ctx->burnout = args->burnout;
  ctx->compile = args->compile;
  ctx->debug = args->debug;
  ctx->refactor = args->refactor;
  ctx->required = args->required;
  ctx->cooldown = args->cooldown;
  ctx->scheduler = args->scheduler;
  ctx->next_seq = 0;
  ctx->stop_flag = false;
  ctx->dongles = NULL;
  // ctx->boss = NULL;
  ctx->coders = NULL;
  ctx->queue = NULL;
  ctx->mon = NULL;
  return 0;
}

int init_context(t_Args *args, t_SharedContext *ctx)
{
  if(init_shared_context(args, ctx) != 0
     || alloc_dongle_array(ctx) != 0
     || alloc_monitor(ctx) != 0
     || alloc_coder_array(ctx) != 0
     // || alloc_queue(ctx) != 0
     || alloc_heapqueue(ctx) != 0
     || init_coder_mutex(ctx) != 0)
    return -1;
  return 0;
}
