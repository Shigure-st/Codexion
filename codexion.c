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

int wallet = 0;
pthread_mutex_t wallet_lock;

void *add_money(void* arg)
{
	for (int i = 0; i < 100000; i++)
	{
		pthread_mutex_lock(&wallet_lock);
		wallet++;
		pthread_mutex_unlock(&wallet_lock);
	}
	return NULL;
}
int	main()
{
	pthread_t	t1, t2;
	pthread_mutex_init(&wallet_lock, NULL);

	pthread_create(&t1, NULL, add_money, NULL);
	pthread_create(&t2, NULL, add_money, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	pthread_mutex_destroy(&wallet_lock);
	printf("[FIX]finall wallet in: %d yen\n", wallet);
	return (0);
}
