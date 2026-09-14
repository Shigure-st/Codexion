#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "codexion.h"

void	free_dongle_heap(t_Dongle *d)
{
	if (d == NULL || d->waiters == NULL)
		return ;
	if (d->waiters->data != NULL)
	{
		free(d->waiters->data);
		d->waiters->data = NULL;
	}
	free(d->waiters);
	d->waiters = NULL;
}

int	heap_pop(t_Dongle *dongle)
{
	t_Heap	*queue;

	queue = dongle->waiters;
	if (queue->size == 0)
	{
		printf("queue is emptyh\n");
		return (-1);
	}
	queue->size--;
	queue->data[0] = queue->data[queue->size];
	shift_down(queue);
	if (queue->size > 0)
		dongle->cond = &queue->data[0].coder->cond;
	else
		dongle->cond = NULL;
	return (0);
}

void	heap_push(t_Dongle *dongle, t_Coder *coder)
{
	t_Heap		*queue;
	long long	t;

	pthread_mutex_lock(&(dongle->lock));
	queue = dongle->waiters;
	queue->data[queue->size].coder = coder;
	if (strcmp(coder->ctx->scheduler, "fifo") == 0)
		queue->data[queue->size].priority = coder->ctx->next_seq++;
	else
	{
		t = get_last_compile_time(coder);
		queue->data[queue->size].priority = t + coder->ctx->burnout;
	}
	shift_up(queue);
	queue->size++;
	if (queue->data[0].coder == coder)
		dongle->cond = &coder->cond;
	pthread_mutex_unlock(&(dongle->lock));
}

int	alloc_heapqueue(t_SharedContext *ctx)
{
	int			i;
	t_Dongle	*d;

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
	return (0);
}
