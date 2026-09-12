#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "codexion.h"


void free_dongle_heap(t_Dongle *d)
{
  if (d == NULL || d->waiters == NULL)
    return;
  if (d->waiters->data != NULL)
  {
    free(d->waiters->data);
    d->waiters->data = NULL;
  }
  free(d->waiters);
  d->waiters = NULL;
}

static void shift_up(t_Heap *queue)
{
  int parent;
  int curent;
  t_HeapData tmp;

  curent = queue->size;
  parent = (curent - 1) / 2;
  while (curent != 0 && queue->data[parent].priority > queue->data[curent].priority)
  {
    tmp = queue->data[parent];
    queue->data[parent] = queue->data[curent];
    queue->data[curent] = tmp;
    curent = parent;
    parent = (curent - 1) / 2;
  }
}

static int get_min_child(t_Heap *queue, int left, int right)
{
  if (right >= queue->size)
    return left;
  else
  {
    if (queue->data[left].priority < queue->data[right].priority)
      return left;
    else
      return right;
  }
}

static void shift_down(t_Heap *queue)
{
  int parent;
  int child;
  t_HeapData tmp;

  parent = 0;
  while((parent * 2) + 1 < queue->size)
  {
    // left_child = (parent * 2) + 1;
    // right_child = (parent * 2) + 2;
    //
    // if (right_child >= queue->size)
    //   child = left_child;
    // else
    // {
    //   if (queue->data[left_child].data < queue->data[right_child].data)
    //     child = left_child;
    //   else
    //     child = right_child;
    // }
    child = get_min_child(queue, (parent * 2) + 1, (parent * 2) + 2);
    if (queue->data[parent].priority > queue->data[child].priority)
    {
      tmp = queue->data[parent];
      queue->data[parent] = queue->data[child];
      queue->data[child] = tmp;
      parent = child;
    }
    else
      break;
  }
}

int heap_pop(t_Dongle *dongle)
{
  t_Heap *queue;

  queue = dongle->waiters;
  if (queue->size == 0)
  {
    printf("queue is emptyh\n");
    return -1;
  }
  queue->size--;
  queue->data[0] = queue->data[queue->size];
  shift_down(queue);
  if (queue->size > 0)
    dongle->cond = &queue->data[0].coder->cond;
  else
    dongle->cond = NULL;
  return 0;
}

void heap_push(t_Dongle *dongle, t_Coder *coder)
{
  t_Heap *queue;

  pthread_mutex_lock(&(dongle->lock));
  queue = dongle->waiters;
  queue->data[queue->size].coder = coder;
  if (strcmp(coder->ctx->scheduler, "fifo") == 0)
    queue->data[queue->size].priority = coder->ctx->next_seq++;
  else
    queue->data[queue->size].priority = coder->t_last;
  shift_up(queue);
  queue->size++;
  if (queue->data[0].coder == coder)
    dongle->cond = &coder->cond;
  pthread_mutex_unlock(&(dongle->lock));

}

int alloc_heapqueue(t_SharedContext *ctx)
{
  int i;
  t_Dongle *d;

  i = 0;
  while (i < ctx->coder)
  {
    d = &ctx->dongles[i];
    d->waiters = malloc(sizeof(t_Heap));
    if (d->waiters == NULL)
      return (cleanup_context(ctx));
    d->waiters->data = malloc(sizeof(t_HeapData) * 2);
    if (d->waiters->data == NULL)
      return (cleanup_context(ctx));
    i++;
  }
  return 0;
}
