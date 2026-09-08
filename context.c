#include <stdbool.h>
#include <pthread.h>
#include "codexion.h"

static int init_shared_context(t_Args *args, t_SharedContext *shared_ctx)
{
	if(pthread_cond_init(&shared_ctx->cond, NULL) != 0)
      return -1;
  shared_ctx->coder = args->coder;
  shared_ctx->burnout = args->burnout;
  shared_ctx->compile = args->compile;
  shared_ctx->debug = args->debug;
  shared_ctx->refactor = args->refactor;
  shared_ctx->required = args->required;
  shared_ctx->cooldown = args->cooldown;
  shared_ctx->scheduler = args->scheduler;
  shared_ctx->next_seq = 0;
  shared_ctx->stop_flag = false;
  shared_ctx->dongles = NULL;
  shared_ctx->boss = NULL;
  shared_ctx->coders = NULL;
  shared_ctx->queue = NULL;
  shared_ctx->monitor = NULL;
  return 0;

  // printf("number_of_coders:%d\n", shared_ctx->coders);
  // printf("time_to_burnout:%d\n", shared_ctx->burnout);
  // printf("time_to_compile:%d\n", shared_ctx->compile);
  // printf("time_to_debug:%d\n", shared_ctx->debug);
  // printf("time_to_refactor:%d\n", shared_ctx->refactor);
  // printf("number_of_compiles_required:%d\n", shared_ctx->required);
  // printf("dongle_cooldown:%d\n", shared_ctx->cooldown);
  // printf("scheduler:%s\n", shared_ctx->scheduler);

}

int init_context(t_Args *args, t_SharedContext *shared_ctx)
{
  if(init_shared_context(args, shared_ctx) != 0
     || alloc_dongle_array(shared_ctx) != 0
     || alloc_boss(shared_ctx) != 0
     || alloc_monitor(shared_ctx) != 0
     || alloc_coder_array(shared_ctx) != 0
     || alloc_queue(shared_ctx) != 0
     || alloc_heapqueue(shared_ctx) != 0
     || init_coder_mutex(shared_ctx) != 0)
    return -1;
  return 0;
}
