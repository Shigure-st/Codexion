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

