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


// int	is_debug(t_Coder *coder)
// {
//   long  total_usec;
//   long  remainder_usec;
//   long   time;
//
//   gettimeofday(&coder->tv, NULL);
//   time = coder->tv.tv_sec;
//   total_usec = coder->tv.tv_usec + (coder->shared_ctx->debug * 1000);
//   coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
//   remainder_usec = total_usec % 1000000;
//   coder->ts.tv_nsec = remainder_usec * 1000;
// 	// printf("Coder %d:Now Debug....\n", coder->number);
//   pthread_mutex_lock(&(coder->local_mutex));
//   pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
//   pthread_mutex_unlock(&coder->local_mutex);
//   if (coder->shared_ctx->stop_flag)
//     return -1;
//   printf("デバックにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
//   return 0;
// }
//
// int	is_refactor(t_Coder *coder)
// {
//   long  total_usec;
//   long  remainder_usec;
//   long   time;
//
//   gettimeofday(&coder->tv, NULL);
//   time = coder->tv.tv_sec;
//   total_usec = coder->tv.tv_usec + (coder->shared_ctx->refactor * 1000);
//   coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
//   remainder_usec = total_usec % 1000000;
//   coder->ts.tv_nsec = remainder_usec * 1000;
// 	// printf("Coder %d:Now Refactoring....\n", coder->number);
//   pthread_mutex_lock(&(coder->local_mutex));
//   pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
//   pthread_mutex_unlock(&coder->local_mutex);
//   if (coder->shared_ctx->stop_flag)
//     return -1;
//   printf("リファクタリングにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
//   return 0;
// }
//
// int	is_compile(t_Coder *coder)
// {
//   long  total_usec;
//   long  remainder_usec;
//   // long   time;
//
//   coder->last_compile_time = get_time_in_ms();
//   // pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
//   // pthread_mutex_lock(&(coder->left_dongle->dongle_lock));
//
//   gettimeofday(&coder->tv, NULL);
//   pthread_mutex_lock(&(coder->monitor->burnout_mutex));
//   pthread_mutex_unlock(&(coder->monitor->burnout_mutex));
//   // time = coder->tv.tv_sec;
//   total_usec = coder->tv.tv_usec + (coder->shared_ctx->compile * 1000);
//   coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
//   remainder_usec = total_usec % 1000000;
//   coder->ts.tv_nsec = remainder_usec * 1000;
//   // printf("test time%ld\n", time);
//
//   pthread_mutex_lock(&(coder->local_mutex));
//   pthread_cond_timedwait(&coder->check_compile_cond, &coder->local_mutex, &coder->ts);
//   pthread_mutex_unlock(&coder->local_mutex);
//   // printf("コンパイルにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
//   // // printf("マイクロ秒:%ld\n", (long)coder->tv.tv_usec);
//   //
//   // printf("Coder Number:%d\n", coder->number);
//   // printf("Coder %d, RightHandDongle:%p\n", coder->number, (void *)&coder->right_dongle->dongle_lock);
//   // printf("Coder %d, LeftHandDongle:%p\n", coder->number, (void *)&coder->left_dongle->dongle_lock);
//   // printf("Coder %d:Now Compile....\n", coder->number);
//   pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
//   pthread_mutex_lock(&(coder->left_dongle->dongle_lock));
//   printf("[DEBUG] coder compile coder:%d\n", coder->number);
//   coder->right_dongle->cooldown_end_time = get_time_in_ms() + coder->shared_ctx->cooldown;
//   coder->left_dongle->cooldown_end_time = get_time_in_ms() + coder->shared_ctx->cooldown;
//   // pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
//   // pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
//   // pthread_mutex_lock(&(coder->boss->request_mutex));
//   (coder->right_dongle->available) = true;
//   (coder->left_dongle->available) = true;
//   pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
//   pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
//   if (coder->right_dongle->coder_cond != NULL)
//     pthread_cond_broadcast(coder->right_dongle->coder_cond);
//   if (coder->left_dongle->coder_cond != NULL)
//     pthread_cond_broadcast(coder->left_dongle->coder_cond);
//   // pthread_cond_broadcast(&(coder->boss->shared_ctx->cond));
//   // pthread_mutex_unlock(&(coder->boss->request_mutex));
//   if (coder->shared_ctx->stop_flag)
//     return -1;
//   return 0;
// }

bool  check_complete(t_SharedContext *ctx)
{
  int i;

  i = 0;
  while (i < ctx->coder)
  {
    if (!ctx->coders[i].done)
      return false;
    // printf("[DEBUG check]\n");
    i++;
  }
  // pthread_mutex_lock(&(shared_ctx->boss->request_mutex));
  ctx->stop_flag = true;
  // pthread_mutex_unlock(&(shared_ctx->boss->request_mutex));
  return true;
}

// bool is_dongle_availble(t_Coder *coder)
// {
//   long long now;
//   t_Dongle  *right;
//   t_Dongle  *left;
//
//   now = get_time_in_ms();
//   right = coder->r_dongle;
//   left = coder->l_dongle;
//   // printf("now:%lld\n", now);
//   // printf("dongle right:%lld\n", right->cooldown_end_time);
//   // printf("dongle left:%lld\n", left->cooldown_end_time);
//   // printf("dongle right bool:%d\n", right->available);
//   // printf("dongle left bool:%d\n", left->available);
//   // printf("Dongle available Coder %d, RightHandDongle:%p\n", coder->number, (void *)&right->dongle_lock);
//   // printf("Dongle available Coder %d, LeftHandDongle:%p\n", coder->number, (void *)&left->dongle_lock);
//   if (now >= right->t_end && now >= left->t_end)
//   {
//     // printf("[DEBUG] dongle_abailble aaaaaaaaaaaaa\n");
//     if (coder->l_dongle->free && coder->r_dongle->free)
//       return true;
//   }
//   return false;
// }

// long long get_cooldown_time(t_Dongle *right, t_Dongle *left)
// {
//   if (right->cooldown_end_time >= left->cooldown_end_time)
//     return right->cooldown_end_time;
//   else
//     return left->cooldown_end_time;
// }

// struct timespec ms_to_timespec(long long ms)
// {
//   struct timespec  ts;
//
//   ts.tv_sec = ms / 1000;
//   ts.tv_nsec = (ms % 1000) * 1000000;
//
//   return ts;
// }

// void  *receive_from_coder(void* arg)
// {
// 	struct s_Boss	*boss;
//   struct s_Coder *coder;
//   long long cooldown;
//
// 	boss = arg;
//   while(!boss->shared_ctx->stop_flag)
//   {
//       pthread_mutex_lock(&(boss->request_mutex));
//       while(is_queue_empty(boss->shared_ctx->queue) && !boss->shared_ctx->stop_flag)
//         pthread_cond_wait(&(boss->shared_ctx->queue->not_empty), &(boss->request_mutex));
//       if (boss->shared_ctx->stop_flag)
//       {
//         pthread_mutex_unlock(&(boss->request_mutex));
//         break;
//       }
//       coder = dequeue(boss->shared_ctx->queue);
//       // printf("[DEBUG] boss dequeue coder:%d\n", coder->number);
//       while(!is_dongle_availble(coder) && !boss->shared_ctx->stop_flag)
//       {
//         cooldown = get_cooldown_time(coder->right_dongle, coder->left_dongle);
//         if (get_time_in_ms() < cooldown)
//         {
//           coder->ts = ms_to_timespec(cooldown);
//           pthread_cond_timedwait(&coder->shared_ctx->cond, &boss->request_mutex, &(coder->ts));
//         }
//         else
//           pthread_cond_wait(&(coder->shared_ctx->cond), &(boss->request_mutex));
//
//       }
//       // printf("[DEBUG] boss dequeue after:%d\n", coder->number);
//       if (boss->shared_ctx->stop_flag)
//       {
//         pthread_mutex_unlock(&(boss->request_mutex));
//         break;
//       }
//       coder->left_dongle->available = false;
//       coder->right_dongle->available = false;
//       coder->wait_cond = true;
//       pthread_cond_broadcast(&(coder->check_compile_cond));
//       pthread_mutex_unlock(&(boss->request_mutex));
//   }
//   return NULL;
// }

// bool is_ready_to_take(t_Dongle *dongle, t_Coder *coder)
// {
//   bool heap_top;
//   bool heap_empty;
//   bool cooldown_over;
//   bool ready;
//
//   // pthread_mutex_lock(&(dongle->dongle_lock));
//   heap_empty = (dongle->wait_coders->size == 0);
//   heap_top = (!heap_empty && dongle->wait_coders->data[0].coder == coder);
//   cooldown_over = (get_time_in_ms() >= dongle->cooldown_end_time);
//   ready = (dongle->available && cooldown_over && (heap_top || heap_empty));
//   // pthread_mutex_unlock(&(dongle->dongle_lock));
//   return ready;
// }

// struct timespec wakeup_time(t_Coder *coder)
// {
//   long long now;
//   long long target;
//
//   now = get_time_in_ms();
//   target = now + coder->shared_ctx->compile + coder->shared_ctx->cooldown;
//   if (coder->right_dongle->available
//      && coder->right_dongle->cooldown_end_time > now)
//     target = coder->right_dongle->cooldown_end_time;
//   if (coder->left_dongle->available
//      && coder->left_dongle->cooldown_end_time > now
//      && coder->left_dongle->cooldown_end_time < target)
//     target = coder->left_dongle->cooldown_end_time;
//   return (ms_to_timespec(target));
// }

// bool try_to_acquire(t_Coder *coder)
// {
//   bool ok;
//
//   pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
//   pthread_mutex_lock(&(coder->left_dongle->dongle_lock));
//   ok = (is_ready_to_take(coder->right_dongle, coder)
//        && is_ready_to_take(coder->left_dongle, coder));
//   if (ok)
//   {
//     heap_pop(coder->right_dongle);
//     heap_pop(coder->left_dongle);
//     coder->right_dongle->available = false;
//     coder->left_dongle->available = false;
//   }
//   pthread_mutex_unlock(&(coder->right_dongle->dongle_lock));
//   pthread_mutex_unlock(&(coder->left_dongle->dongle_lock));
//   return ok;
// }

// void acquire_dongles(t_Coder *coder)
// {
//   struct timespec wakeup;
//   bool acquire;
//
//   acquire = false;
//   if (is_empty_and_free(coder->right_dongle)
//       && is_empty_and_free(coder->left_dongle))
//   {
//     pthread_mutex_lock(&(coder->right_dongle->dongle_lock));
//     pthread_mutex_lock(&(coder->left_dongle->dongle_lock));
//     coder->right_dongle->available = false;
//     coder->left_dongle->available = false;
//     pthread_mutex_unlock(&(coder->right_dongle->dongle_lock));
//     pthread_mutex_unlock(&(coder->left_dongle->dongle_lock));
//   }
//   else
//   {
//     if (!is_empty_and_free(coder->right_dongle))
//       heap_push(coder->right_dongle, coder);
//     if (!is_empty_and_free(coder->left_dongle))
//       heap_push(coder->left_dongle, coder);
//     pthread_mutex_lock(&(coder->local_mutex));
//     while (!acquire)
//     {
//       wakeup = wakeup_time(coder);
//       pthread_cond_timedwait(&(coder->check_compile_cond), &(coder->local_mutex), &wakeup);
//       acquire = try_to_acquire(coder);
//     }
//     pthread_mutex_unlock(&(coder->local_mutex));
//   }
//   return;
// }

// void	*simulate(void* arg)
// {
// 	struct s_Coder	*coder;
//   int i;
//
// 	coder = arg;
//   coder->t_last = get_time_in_ms();
//   if (coder->r_dongle == coder->l_dongle)
//   {
//     pthread_mutex_lock(&(coder->lock));
//     while (!coder->ctx->stop_flag)
//       pthread_cond_wait(&(coder->cond), &(coder->lock));
//     pthread_mutex_unlock(&(coder->lock));
//     return NULL;
//   }
//   i = 0;
//   while(i < coder->ctx->required)
//   {
//     acquire_dongles(coder);
//     if (coder->ctx->stop_flag || is_compile(coder) == -1)
//       break;
//     if (coder->ctx->stop_flag || is_debug(coder) == -1)
//       break;
//     if (coder->ctx->stop_flag || is_refactor(coder) == -1)
//       break;
//     printf("Coder:%d compile number:%d\n", coder->id, i + 1);
//     i++;
//   }
//   printf("compile complete\n");
//   coder->done = true;
//   if (check_complete(coder->ctx))
//     pthread_cond_broadcast(&(coder->shared_ctx->queue->not_empty));
//   return NULL;
// }


// long long  get_time_in_ms(void)
// {
//   struct timeval  tv;
//   long long time;
//
//   gettimeofday(&tv, NULL);
//   time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
//   return time;
// }
