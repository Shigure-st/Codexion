#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "codexion.h"
#include <time.h>
#include <sys/time.h>


int	is_debug(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;
  long   time;

  gettimeofday(&coder->tv, NULL);
  time = coder->tv.tv_sec;
  total_usec = coder->tv.tv_usec + (coder->ctx->debug * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;
	// printf("Coder %d:Now Debug....\n", coder->number);
  pthread_mutex_lock(&(coder->lock));
  pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
  pthread_mutex_unlock(&coder->lock);
  if (coder->ctx->stop_flag)
    return -1;
  printf("デバックにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
  return 0;
}

int	is_refactor(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;
  long   time;

  gettimeofday(&coder->tv, NULL);
  time = coder->tv.tv_sec;
  total_usec = coder->tv.tv_usec + (coder->ctx->refactor * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;
	// printf("Coder %d:Now Refactoring....\n", coder->number);
  pthread_mutex_lock(&(coder->lock));
  pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
  pthread_mutex_unlock(&coder->lock);
  if (coder->ctx->stop_flag)
    return -1;
  printf("リファクタリングにかかった秒数:%ld\n", ((long)coder->ts.tv_sec - time));
  return 0;
}

int	is_compile(t_Coder *coder)
{
  long  total_usec;
  long  remainder_usec;

  coder->t_last = get_time_in_ms();

  gettimeofday(&coder->tv, NULL);
  pthread_mutex_lock(&(coder->mon->lock));
  pthread_mutex_unlock(&(coder->mon->lock));
  total_usec = coder->tv.tv_usec + (coder->ctx->compile * 1000);
  coder->ts.tv_sec = coder->tv.tv_sec + (total_usec / 1000000);
  remainder_usec = total_usec % 1000000;
  coder->ts.tv_nsec = remainder_usec * 1000;

  pthread_mutex_lock(&(coder->lock));
  pthread_cond_timedwait(&coder->cond, &coder->lock, &coder->ts);
  pthread_mutex_unlock(&coder->lock);
  pthread_mutex_lock(&(coder->r_dongle->lock));
  pthread_mutex_lock(&(coder->l_dongle->lock));
  printf("[DEBUG] coder compile coder:%d\n", coder->id);
  coder->r_dongle->t_end = get_time_in_ms() + coder->ctx->cooldown;
  coder->l_dongle->t_end = get_time_in_ms() + coder->ctx->cooldown;
  (coder->r_dongle->free) = true;
  (coder->l_dongle->free) = true;
  pthread_mutex_unlock(&coder->r_dongle->lock);
  pthread_mutex_unlock(&coder->l_dongle->lock);
  if (coder->r_dongle->cond != NULL)
    pthread_cond_broadcast(coder->r_dongle->cond);
  if (coder->l_dongle->cond != NULL)
    pthread_cond_broadcast(coder->l_dongle->cond);
  if (coder->ctx->stop_flag)
    return -1;
  return 0;
}

void acquire_dongles(t_Coder *coder)
{
  struct timespec wakeup;
  bool acquire;

  acquire = false;
  if (is_empty_and_free(coder->r_dongle)
      && is_empty_and_free(coder->l_dongle))
  {
    pthread_mutex_lock(&(coder->r_dongle->lock));
    pthread_mutex_lock(&(coder->l_dongle->lock));
    coder->r_dongle->free = false;
    coder->l_dongle->free = false;
    pthread_mutex_unlock(&(coder->r_dongle->lock));
    pthread_mutex_unlock(&(coder->l_dongle->lock));
  }
  else
  {
    if (!is_empty_and_free(coder->r_dongle))
      heap_push(coder->r_dongle, coder);
    if (!is_empty_and_free(coder->l_dongle))
      heap_push(coder->l_dongle, coder);
    pthread_mutex_lock(&(coder->lock));
    while (!acquire)
    {
      wakeup = wakeup_time(coder);
      pthread_cond_timedwait(&(coder->cond), &(coder->lock), &wakeup);
      acquire = try_to_acquire(coder);
    }
    pthread_mutex_unlock(&(coder->lock));
  }
  return;
}
