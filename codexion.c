/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:00:20 by tenomoto          #+#    #+#             */
/*   Updated: 2026/06/22 16:39:32 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct s_SharedContext t_SharedContext;
typedef struct s_Coder t_Coder;
typedef struct s_Dongle t_Dongle;
typedef struct s_Monitor t_Monitor;
typedef struct s_Boss t_Boss;
typedef struct s_Data t_Data;
typedef struct s_Queue t_Queue;

struct s_Queue
{
  struct s_Data *arr;
  int           tail;
  int           head;
  int           size;
};

struct s_Data
{
  struct s_Coder  *coder;
  int  priority;
};

struct s_Boss
{
  struct s_Queue  *queue;
  pthread_mutex_t request_mutex;
  bool            request_flag;
	struct s_SharedContext	*shared_ctx;
};

struct s_SharedContext
{
  int			number_of_coders;
  int			time_to_debug;
  int			time_to_refactor;
  int			time_to_compile;
  bool		is_active;
	t_Dongle	*dongle_array;
	t_Coder		*coder_array;
  pthread_cond_t  cond;
};

struct s_Dongle
{
	int				i;
	pthread_mutex_t dongle_lock;
	bool			available;
};

struct s_Coder
{
	int				number;
  struct s_Queue  *queue;
	struct s_Dongle	*right_hand_dongle;
	struct s_Dongle	*left_hand_dongle;
  struct s_Monitor *monitor_thread;
  struct s_Boss *boss_thread;
	struct s_SharedContext	*shared_ctx;
	bool			is_compile;
};

struct s_Monitor
{
  pthread_mutex_t request_mutex;
};

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

void	is_debug(int	number)
{
	usleep(3000);
	printf("Coder %d:Now Debug....\n", number);
}

void	is_refactor(int	number)
{
	usleep(3000);
	printf("Coder %d:Now Refactoring....\n", number);
}

void	is_compile(t_Coder *coder)
{
  // if (coder->right_hand_dongle->available && coder->left_hand_dongle->available)
  // {
  pthread_mutex_lock(&(coder->right_hand_dongle->dongle_lock));
  pthread_mutex_lock(&(coder->left_hand_dongle->dongle_lock));
  printf("Coder Number:%d\n", coder->number);
  printf("Coder %d, RightHandDongle:%p\n", coder->number, (void *)&coder->right_hand_dongle->dongle_lock);
  printf("Coder %d, LeftHandDongle:%p\n", coder->number, (void *)&coder->left_hand_dongle->dongle_lock);
  printf("Coder %d:Now Compile....\n", coder->number);
  pthread_mutex_unlock(&coder->right_hand_dongle->dongle_lock);
  pthread_mutex_unlock(&coder->left_hand_dongle->dongle_lock);
  pthread_mutex_lock(&(coder->boss_thread->request_mutex));
  (coder->right_hand_dongle->available) = true;
  (coder->left_hand_dongle->available) = true;
  pthread_mutex_unlock(&(coder->boss_thread->request_mutex));
  usleep(10000);
  is_debug(coder->number);
  is_refactor(coder->number);
  // }

}

void  *receive_from_coder(void* arg)
{
	struct s_Boss	*boss;
  struct s_Coder *coder;
	boss = arg;
    while(1)
  {
      if(!is_queue_empty(boss->queue))
      {
        pthread_mutex_lock(&(boss->request_mutex));
        coder = dequeue(boss->queue);
        if (coder && coder->left_hand_dongle->available && coder->right_hand_dongle->available)
        {
          coder->left_hand_dongle->available = false;
          coder->right_hand_dongle->available = false;
          coder->is_compile = true;
          pthread_cond_broadcast(&(boss->shared_ctx->cond));
        }
        else if(coder)
          enqueue(coder->queue, coder);
        boss->request_flag = false;
        pthread_mutex_unlock(&(boss->request_mutex));
      }
  }
}


void	*simulate(void* arg)
{
	// printf("Coder Number:%d\n", ((struct s_Coder *)arg)->number);
	struct s_Coder	*coder;
	coder = arg;
  while(1)
  {
    usleep(1000000);
    pthread_mutex_lock(&(coder->boss_thread->request_mutex));
    enqueue(coder->queue, coder);
    coder->boss_thread->request_flag = true;
    while(!coder->is_compile)
      pthread_cond_wait(&(coder->shared_ctx->cond), &(coder->boss_thread->request_mutex));
    pthread_mutex_unlock(&(coder->boss_thread->request_mutex));
    coder->is_compile = false;
    is_compile(coder);
  }
    // is_compile(coder);
	// is_debug();
	// is_refactor();

	return NULL;
}

int	main()
{
	struct s_SharedContext shared_ctx;
  pthread_cond_init(&shared_ctx.cond, NULL);

	struct s_Dongle dongle_array[2];
	struct s_Coder coder_array[2];
  struct s_Data dates[3] = {0};

  struct  s_Queue heap_queue;
  heap_queue.arr = dates;
  heap_queue.size = 3;
  heap_queue.head = 0;
  heap_queue.tail = -1;


	shared_ctx.number_of_coders = 2;
	shared_ctx.time_to_debug = 200;
	shared_ctx.time_to_refactor = 200;
	shared_ctx.time_to_compile = 200;
	shared_ctx.is_active = true;

  struct s_Boss boss_thread;
  boss_thread.request_flag = false;
  boss_thread.queue = &heap_queue;
  boss_thread.shared_ctx = &shared_ctx;

	pthread_mutex_init(&dongle_array[0].dongle_lock, NULL);
	pthread_mutex_init(&dongle_array[1].dongle_lock, NULL);
  pthread_mutex_init(&boss_thread.request_mutex, NULL);


	dongle_array[0].available = true;
	dongle_array[1].available = true;

	coder_array[0].number = 1;
	coder_array[0].left_hand_dongle = &dongle_array[0];
	coder_array[0].right_hand_dongle = &dongle_array[1];
	coder_array[0].shared_ctx = &shared_ctx;
	coder_array[0].boss_thread = &boss_thread;
	coder_array[0].queue = &heap_queue;
	coder_array[0].is_compile = false;

	coder_array[1].number = 2;
	coder_array[1].right_hand_dongle = &dongle_array[0];
	coder_array[1].left_hand_dongle = &dongle_array[1];
	coder_array[1].shared_ctx = &shared_ctx;
	coder_array[1].boss_thread = &boss_thread;
	coder_array[1].queue = &heap_queue;
	coder_array[1].is_compile = false;

	// printf("Nnmber_of_coders:%d\n", shared_ctx.number_of_coders);
	// printf("coder_array[0]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[0].number, coder_array[0].left_hand_dongle, coder_array[0].right_hand_dongle);
	// printf("coder_array[0] shared_ctx.time_to_debug %d\n", coder_array[0].shared_ctx->time_to_debug);
	// printf("coder_array[1]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[1].number, coder_array[1].left_hand_dongle, coder_array[1].right_hand_dongle);

	pthread_t	t_coder1;
	pthread_t	t_coder2;
  pthread_t t_boss;

	pthread_create(&t_coder1, NULL, simulate, &coder_array[0]);
	pthread_create(&t_coder2, NULL, simulate, &coder_array[1]);
  pthread_create(&t_boss, NULL, receive_from_coder, &boss_thread);

	pthread_join(t_coder1, NULL);
	pthread_join(t_coder2, NULL);
  pthread_join(t_boss, NULL);
	return (0);
}
