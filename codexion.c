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

void *sub_worker(void* arg)
{
	for (int i = 0; i < 5; i++)
	{
		printf("[sub]work! (%d)\n", i + i);
		usleep(500000);
	}
	printf("[sub]finish\n");
	return NULL;
}
int	main()
{
	pthread_t	thread_id;
	printf("[main]employ sub_thread... \n");

	pthread_create(&thread_id, NULL, sub_worker, NULL);

	for (int i = 0; i < 3; i++)
	{
		printf("[main] other work...(%d)\n", i + 1);
		usleep(700000);
	}

	printf("[main] wait sub_thread...\n");

	pthread_join(thread_id, NULL);
	printf("[main]all process complete!\n");
	return (0);
}
