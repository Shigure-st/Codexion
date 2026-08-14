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
#include "codexion.h"
#include <time.h>
#include <sys/time.h>


void	is_debug(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;
  long   time;

  gettimeofday(&coder->tv, NULL);
  time = coder->tv.tv_sec;
  total_usec = coder->tv.tv_usec + (coder->shared_ctx->debug * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;
	printf("Coder %d:Now Debug....\n", coder->number);
  pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
  printf("デバックにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
}

void	is_refactor(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;
  long   time;

  gettimeofday(&coder->tv, NULL);
  time = coder->tv.tv_sec;
  total_usec = coder->tv.tv_usec + (coder->shared_ctx->refactor * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;
	printf("Coder %d:Now Refactoring....\n", coder->number);
  pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
  printf("リファクタリングにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
}

void	is_compile(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;
  long   time;

  pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
  pthread_mutex_lock(&(coder->left_dongle->dongle_lock));

  gettimeofday(&coder->tv, NULL);
  time = coder->tv.tv_sec;
  total_usec = coder->tv.tv_usec + (coder->shared_ctx->compile * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;
  printf("test time%ld\n", time);

  pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
  printf("コンパイルにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
  // printf("マイクロ秒:%ld\n", (long)coder->tv.tv_usec);

  printf("Coder Number:%d\n", coder->number);
  printf("Coder %d, RightHandDongle:%p\n", coder->number, (void *)&coder->right_dongle->dongle_lock);
  printf("Coder %d, LeftHandDongle:%p\n", coder->number, (void *)&coder->left_dongle->dongle_lock);
  printf("Coder %d:Now Compile....\n", coder->number);
  pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
  pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
  pthread_mutex_lock(&(coder->boss->request_mutex));
  (coder->right_dongle->available) = true;
  (coder->left_dongle->available) = true;
  pthread_cond_broadcast(&(coder->boss->shared_ctx->cond));
  pthread_mutex_unlock(&(coder->boss->request_mutex));
}

bool  check_complete(t_SharedContext *shared_ctx)
{
  int i;

  i = 0;
  while (i < shared_ctx->coder)
  {
    if (!shared_ctx->coders[i].is_complete)
      return false;
    printf("[DEBUG check]\n");
    i++;
  }
  pthread_mutex_lock(&(shared_ctx->boss->request_mutex));
  shared_ctx->stop_flag = true;
  pthread_mutex_unlock(&(shared_ctx->boss->request_mutex));
  return true;
}

void  *receive_from_coder(void* arg)
{
	struct s_Boss	*boss;
  struct s_Coder *coder;
	boss = arg;
  while(!boss->shared_ctx->stop_flag)
  {
      pthread_mutex_lock(&(boss->request_mutex));
      while(is_queue_empty(boss->shared_ctx->queue) && !boss->shared_ctx->stop_flag)
        pthread_cond_wait(&(boss->shared_ctx->queue->not_empty), &(boss->request_mutex));
      if (boss->shared_ctx->stop_flag)
      {
        pthread_mutex_unlock(&(boss->request_mutex));
        break;
      }
      coder = dequeue(boss->shared_ctx->queue);
      while((!coder->left_dongle->available || !coder->right_dongle->available) && !boss->shared_ctx->stop_flag)
        pthread_cond_wait(&(coder->shared_ctx->cond), &(coder->boss->request_mutex));
      if (boss->shared_ctx->stop_flag)
      {
        pthread_mutex_unlock(&(boss->request_mutex));
        break;
      }
      coder->left_dongle->available = false;
      coder->right_dongle->available = false;
      coder->wait_cond = true;
      pthread_cond_broadcast(&(coder->check_compile_cond));
      pthread_mutex_unlock(&(boss->request_mutex));
  }
  return NULL;
}


void	*simulate(void* arg)
{
	struct s_Coder	*coder;
  int i;
  i = 0;
	coder = arg;
  while(i < coder->shared_ctx->required)
  {
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
    pthread_mutex_unlock(&(coder->boss->request_mutex));
    is_compile(coder);
    is_debug(coder);
    is_refactor(coder);
    printf("Coder:%d compile number:%d\n", coder->number, i);
    i++;
  }
  printf("compile complete\n");
  coder->is_complete = true;
  if (check_complete(coder->shared_ctx))
    pthread_cond_broadcast(&(coder->shared_ctx->queue->not_empty));

	return NULL;
}

