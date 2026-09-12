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


bool  check_complete(t_SharedContext *ctx)
{
  int i;

  i = 0;
  while (i < ctx->coder)
  {
    if (!ctx->coders[i].done)
      return false;
    i++;
  }
  ctx->stop_flag = true;
  return true;
}
