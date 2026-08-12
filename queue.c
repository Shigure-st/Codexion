#include "codexion.h"
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

bool  is_queue_empty(t_Queue *queue)
{
  if((queue->tail + 1) % queue->size == queue->head)
    return true;
  else
    return false;
}

void  enqueue(t_Queue *queue, t_Coder *element)
{
  if((queue->tail + 2) % (queue->size) == queue->head)
  {
    printf("Queue is full so can't ENQUEUE\n");
    return;

  }
  queue->arr[(queue->tail + 1) % queue->size].coder = element;
  queue->tail = (queue->tail + 1) % queue->size;
  pthread_cond_broadcast(&(queue->not_empty));
  // printf("[DEBUG]heap queue:%d\n", queue->arr[(queue->tail) % queue->size].coder);
  // printf("[DEBUG]tail:%d\n", queue->tail);
}

void  *dequeue(t_Queue *queue)
{
  struct s_Coder *ret;

  // printf("[DEBUG]\n");
  // printf("tail:%d\n", queue->tail);
  if((queue->tail + 1) % queue->size == queue->head)
  {
    printf("Queue is empty\n");
    return NULL;
  }
  ret = queue->arr[queue->head].coder;
  queue->head = (queue->head + 1) % queue->size;
  return ret;

}

int alloc_queue(t_SharedContext *shared_ctx)
{
  shared_ctx->queue = malloc(sizeof(t_Queue));
  if (shared_ctx->queue == NULL)
    return (cleanup_context(shared_ctx));
  shared_ctx->queue->arr = malloc(sizeof(t_Data) * (shared_ctx->coder + 1));
  if (shared_ctx->queue->arr == NULL)
  {
    free(shared_ctx->queue);
    shared_ctx->queue = NULL;
    return (cleanup_context(shared_ctx));
  }
  shared_ctx->queue->size = shared_ctx->coder + 1;
  shared_ctx->queue->head = 0;
  shared_ctx->queue->tail = -1;
  if (pthread_cond_init(&shared_ctx->queue->not_empty, NULL) != 0)
  {
    free(shared_ctx->queue->arr);
    free(shared_ctx->queue);
    shared_ctx->queue = NULL;
    return (cleanup_context(shared_ctx));
  }
  return 0;
}
