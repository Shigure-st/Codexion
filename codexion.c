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
#include <string.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct s_Args t_Args;
typedef struct s_SharedContext t_SharedContext;
typedef struct s_Coder t_Coder;
typedef struct s_Dongle t_Dongle;
typedef struct s_Monitor t_Monitor;
typedef struct s_Boss t_Boss;
typedef struct s_Data t_Data;
typedef struct s_Queue t_Queue;

struct s_Args
{
  int   coder;
  int   burnout;
  int   compile;
  int   debug;
  int   refactor;
  int   required;
  int   cooldown;
  char  *scheduler;
};

struct s_Queue
{
  struct s_Data *arr;
  int           tail;
  int           head;
  int           size;
  pthread_cond_t  not_empty;
};

struct s_Data
{
  struct s_Coder  *coder;
  int  priority;
};

struct s_Boss
{
  struct s_Queue  *queue;
	pthread_t	      t_Boss;
  pthread_mutex_t request_mutex;
  bool            request_flag;
	struct s_SharedContext	*shared_ctx;
};

struct s_SharedContext
{
  int			coder;
  int			burnout;
  int			debug;
  int			refactor;
  int			compile;
  int     required;
  int     cooldown;
  char  *scheduler;
  bool		is_active;
  bool		stop_flag;
	t_Dongle	*dongles;
	t_Coder		*coders;
  t_Queue  *queue;
  t_Boss    *boss;
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
	bool			is_compile;
  bool      wait_cond;
	pthread_t	t_Coder;
  pthread_cond_t  check_compile_cond;
  struct s_Queue  *queue;
	struct s_Dongle	*right_dongle;
	struct s_Dongle	*left_dongle;
  struct s_Monitor *monitor_thread;
  struct s_Boss *boss;
	struct s_SharedContext	*shared_ctx;
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
  pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
  pthread_mutex_lock(&(coder->left_dongle->dongle_lock));
  printf("Coder Number:%d\n", coder->number);
  printf("Coder %d, RightHandDongle:%p\n", coder->number, (void *)&coder->right_dongle->dongle_lock);
  printf("Coder %d, LeftHandDongle:%p\n", coder->number, (void *)&coder->left_dongle->dongle_lock);
  printf("Coder %d:Now Compile....\n", coder->number);
  pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
  pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
  pthread_mutex_lock(&(coder->boss->request_mutex));
  (coder->right_dongle->available) = true;
  (coder->left_dongle->available) = true;
  pthread_mutex_unlock(&(coder->boss->request_mutex));
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
  printf("[DEBUG boss]\n");
  while(1)
  {
      // printf("[DEBUG:receive_from_coder]\n");
      pthread_mutex_lock(&(boss->request_mutex));
      while(is_queue_empty(boss->shared_ctx->queue) && !boss->shared_ctx->stop_flag)
        pthread_cond_wait(&(boss->shared_ctx->queue->not_empty), &(boss->request_mutex));
      if (boss->shared_ctx->stop_flag)
      {
        pthread_mutex_unlock(&(boss->request_mutex));
        break;
      }
      coder = dequeue(boss->shared_ctx->queue);
      coder->wait_cond = true;
      pthread_cond_broadcast(&(coder->check_compile_cond));
      // while(!coder->is_compile)
      while(coder->left_dongle->available && coder->right_dongle->available && !boss->shared_ctx->stop_flag)
        pthread_cond_wait(&(coder->shared_ctx->cond), &(coder->boss->request_mutex));
      if (boss->shared_ctx->stop_flag)
      {
        pthread_mutex_unlock(&(boss->request_mutex));
        break;
      }
      // printf("[DEBUG:receive_from_coder end]\n");
      // coder->left_hand_dongle->available = false;
      // coder->right_hand_dongle->available = false;
      coder->is_compile = true;
      pthread_cond_broadcast(&(coder->check_compile_cond));
      // pthread_cond_broadcast(&(boss->shared_ctx->cond));
      pthread_mutex_unlock(&(boss->request_mutex));
  }
  return NULL;
}


void	*simulate(void* arg)
{
	// printf("Coder Number:%d\n", ((struct s_Coder *)arg)->number);
	struct s_Coder	*coder;
	coder = arg;
  while(1)
  {
    // printf("[DEBUG:simulate]\n");
    usleep(1000000);
    pthread_mutex_lock(&(coder->boss->request_mutex));
    enqueue(coder->shared_ctx->queue, coder);
    while(!coder->wait_cond && !coder->shared_ctx->stop_flag)
      pthread_cond_wait(&(coder->check_compile_cond), &(coder->boss->request_mutex));
    if (coder->shared_ctx->stop_flag)
    {
      pthread_mutex_unlock(&(coder->boss->request_mutex));
      break;
    }
    coder->wait_cond = false;
    // pthread_mutex_lock(&(coder->right_hand_dongle->dongle_lock));
    // pthread_mutex_lock(&(coder->left_hand_dongle->dongle_lock));
    coder->left_dongle->available = false;
    coder->right_dongle->available = false;
    pthread_cond_broadcast(&(coder->boss->shared_ctx->cond));
    // coder->is_compile = true;
    // while(!coder->is_compile)
    //   pthread_cond_wait(&(coder->shared_ctx->cond), &(coder->boss_thread->request_mutex));
    while(!coder->is_compile && !coder->shared_ctx->stop_flag)
      pthread_cond_wait(&(coder->check_compile_cond), &(coder->boss->request_mutex));
    if (coder->shared_ctx->stop_flag)
    {
      pthread_mutex_unlock(&(coder->boss->request_mutex));
      break;
    }
    pthread_mutex_unlock(&(coder->boss->request_mutex));
    coder->is_compile = false;
    is_compile(coder);

    // is_compile(coder);
	// is_debug();
	// is_refactor();
  }

	return NULL;
}

int parse_char(char *arg, char **target)
{
  if(strcmp(arg, "fifo") == 0
     || strcmp(arg, "edf") == 0)
    return (*target = arg, 0);
  return 1;
}

int parse_int(char *arg, int *target)
{
  int val;

  val = atoi(arg);
  if(val != 0)
    return (*target = val, 0);
  return 1;
}

int parse_args(int argc, char **argv, t_Args *args)
{

  if(argc != 9)
    return (fprintf(stderr, "Error:Invalid arguments\n"), -1);

  if(parse_int(argv[1], &(args->coder)) != 0
     || parse_int(argv[2], &(args->burnout)) != 0
     || parse_int(argv[3], &(args->compile)) != 0
     || parse_int(argv[4], &(args->debug)) != 0
     || parse_int(argv[5], &(args->refactor)) != 0
     || parse_int(argv[6], &(args->required)) != 0
     || parse_int(argv[7], &(args->cooldown)) != 0)
    return (fprintf(stderr, "Error:Invalid integer arguments\n"), -1);
  if(parse_char(argv[8], &(args->scheduler)) != 0)
    return (fprintf(stderr, "Error:Invalid string arguments\n"), -1);

  // printf("arg1:%d\n", args->coder);
  // printf("arg2:%d\n", args->burnout);
  // printf("arg3:%d\n", args->compile);
  // printf("arg4:%d\n", args->debug);
  // printf("arg5:%d\n", args->refactor);
  // printf("arg6:%d\n", args->required);
  // printf("arg7:%d\n", args->cooldown);
  // printf("arg8:%s\n", args->scheduler);

  return 0;
}

int init_shared_context(t_Args *args, t_SharedContext *shared_ctx)
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
  shared_ctx->stop_flag = false;
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
    return -1;
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
      return -1;
    }
    init_coder_struct(shared_ctx, i);
    i++;
  }
  return 0;
}

int alloc_queue(t_SharedContext *shared_ctx)
{
  shared_ctx->queue = malloc(sizeof(t_Queue));
  if (shared_ctx->queue == NULL)
    return -1;
  shared_ctx->queue->arr = malloc(sizeof(t_Data) * (shared_ctx->coder + 1));
  if (shared_ctx->queue->arr == NULL)
    return (free(shared_ctx->queue), -1);
  shared_ctx->queue->size = shared_ctx->coder + 1;
  shared_ctx->queue->head = 0;
  shared_ctx->queue->tail = -1;
  if (pthread_cond_init(&shared_ctx->queue->not_empty, NULL) != 0)
  {
    free(shared_ctx->queue->arr);
    free(shared_ctx->queue);
    return -1;
  }
  return 0;
}

int alloc_boss(t_SharedContext *shared_ctx)
{
  shared_ctx->boss = malloc(sizeof(t_Boss));
  if (shared_ctx->boss == NULL)
    return -1;
  shared_ctx->boss->shared_ctx = shared_ctx;
  if (pthread_mutex_init(&shared_ctx->boss->request_mutex, NULL) != 0)
    return (free(shared_ctx->boss), -1);
  return 0;
}

int init_context(t_Args *args, t_SharedContext *shared_ctx)
{
  if(init_shared_context(args, shared_ctx) != 0
     || alloc_dongle_array(shared_ctx) != 0
     || alloc_boss(shared_ctx) != 0
     || alloc_coder_array(shared_ctx) != 0
     || alloc_queue(shared_ctx) != 0)
    return -1;
  return 1;
}

void wakeup_all_thread(t_SharedContext *shared_ctx, int coder)
{
  int i;

  i = 0;
  while(i < coder)
  {
      pthread_cond_broadcast(&(shared_ctx->coders[i].check_compile_cond));
      i++;
  }
  pthread_cond_broadcast(&(shared_ctx->cond));
}

int run_simulation(t_SharedContext *shared_ctx)
{
  int i;
  int j;

  i = 0;
  if (pthread_create(&shared_ctx->boss->t_Boss, NULL, receive_from_coder, shared_ctx->boss) != 0)
    return -1;
  while(i < shared_ctx->coder)
  {
    if (pthread_create(&shared_ctx->coders[i].t_Coder, NULL, simulate, &shared_ctx->coders[i]) != 0)
    {
      shared_ctx->stop_flag = true;
      wakeup_all_thread(shared_ctx, i);
      break;
    }
    i++;
  }
  j = 0;
  while(j < i)
    pthread_join(shared_ctx->coders[j++].t_Coder, NULL);
  pthread_join(shared_ctx->boss->t_Boss, NULL);
  return 0;
}

int	main(int argc, char **argv)
{
  struct s_Args args;
	struct s_SharedContext shared_ctx;

  parse_args(argc, argv, &args);
  init_context(&args, &shared_ctx);
  run_simulation(&shared_ctx);
  printf("argc:%d, argv:%s\n", argc, argv[1]);
  pthread_cond_init(&shared_ctx.cond, NULL);

	// struct s_Dongle dongle_array[2];
	// struct s_Coder coder_array[2];
	//  struct s_Data dates[3] = {0};

  // struct  s_Queue heap_queue;
  // heap_queue.arr = dates;
  // heap_queue.size = 3;
  // heap_queue.head = 0;
  // heap_queue.tail = -1;
  // pthread_cond_init(&heap_queue.not_empty, NULL);


	// shared_ctx.number_of_coders = 2;
	// shared_ctx.time_to_debug = 200;
	// shared_ctx.time_to_refactor = 200;
	// shared_ctx.time_to_compile = 200;
	// shared_ctx.is_active = true;

	//  struct s_Boss boss_thread;
	//  boss_thread.request_flag = false;
	//  boss_thread.queue = &heap_queue;
	//  boss_thread.shared_ctx = &shared_ctx;
	//
	// pthread_mutex_init(&dongle_array[0].dongle_lock, NULL);
	// pthread_mutex_init(&dongle_array[1].dongle_lock, NULL);
	//  pthread_mutex_init(&boss_thread.request_mutex, NULL);


	// dongle_array[0].available = true;
	// dongle_array[1].available = true;
	//
	// coder_array[0].number = 1;
	// coder_array[0].left_hand_dongle = &dongle_array[0];
	// coder_array[0].right_hand_dongle = &dongle_array[1];
	// coder_array[0].shared_ctx = &shared_ctx;
	// coder_array[0].boss_thread = &boss_thread;
	// coder_array[0].queue = &heap_queue;
	// coder_array[0].is_compile = false;
	// coder_array[0].wait_cond = false;
	//  pthread_cond_init(&coder_array[0].check_compile_cond, NULL);
	//
	// coder_array[1].number = 2;
	// coder_array[1].right_hand_dongle = &dongle_array[0];
	// coder_array[1].left_hand_dongle = &dongle_array[1];
	// coder_array[1].shared_ctx = &shared_ctx;
	// coder_array[1].boss_thread = &boss_thread;
	// coder_array[1].queue = &heap_queue;
	// coder_array[1].is_compile = false;
	// coder_array[1].wait_cond = false;
	//  pthread_cond_init(&coder_array[1].check_compile_cond, NULL);

	// printf("Nnmber_of_coders:%d\n", shared_ctx.number_of_coders);
	// printf("coder_array[0]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[0].number, coder_array[0].left_hand_dongle, coder_array[0].right_hand_dongle);
	// printf("coder_array[0] shared_ctx.time_to_debug %d\n", coder_array[0].shared_ctx->time_to_debug);
	// printf("coder_array[1]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[1].number, coder_array[1].left_hand_dongle, coder_array[1].right_hand_dongle);

	// pthread_t	t_coder1;
	// pthread_t	t_coder2;
	//  pthread_t t_boss;
	//
	// pthread_create(&t_coder1, NULL, simulate, &coder_array[0]);
	// pthread_create(&t_coder2, NULL, simulate, &coder_array[1]);
	//  pthread_create(&t_boss, NULL, receive_from_coder, &boss_thread);
	//
	// pthread_join(t_coder1, NULL);
	// pthread_join(t_coder2, NULL);
	//  pthread_join(t_boss, NULL);
	return (0);
}
