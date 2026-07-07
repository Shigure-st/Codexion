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

typedef struct s_SharedContext
{
	int			number_of_coders;
	int			time_to_debug;
	int			time_to_refactor;
	int			time_to_compile;
	bool		is_active;
	t_dongle	*dongle_array;
	t_coder		*coder_array;
}	t_SharedContext;

typedef struct s_Dongle
{
	pthread_mutex_t dongle_lock;
}	t_Dongle

typedef struct s_Coder
{
	int				number;
	struct s_Dongle	*right_hand_dongle;
	struct s_Dongle	*left_hand_dongle;
	struct s_SharedContext	*shared_ctx;
}	t_Coder;

void *customer(void* arg)
{
	char	*name = (char*)arg;

	pthread_mutex_lock(&kitchen_lock);
	while (ramen_ready == 0)
	{
		printf("[%s] waiting for ramen... \n", name);
		// pthread_cond_wait(&ramen_cond, &kitchen_lock);
	}
	printf("[%s] oh, coming ramen\n", name);

	pthread_mutex_unlock(&kitchen_lock);
	return NULL;
}

void *chef(void* arg)
{
	printf("[staff] I start to make ramen....\n");
	sleep(5);

	pthread_mutex_lock(&kitchen_lock);
	ramen_ready = 1;
	printf("[staff] welcome ramen\n");
	pthread_cond_broadcast(&ramen_cond);
	pthread_mutex_unlock(&kitchen_lock);
	return NULL;
}
int	main()
{
	struct s_SharedContext shared_ctx
	pthread_t	t_chef, t_customer1, t_customer2;

	pthread_mutex_init(&kitchen_lock, NULL);
	pthread_cond_init(&ramen_cond, NULL);

	pthread_create(&t_customer1, NULL, customer, "Kyaku:A");
	pthread_create(&t_customer2, NULL, customer, "Kyaku:B");
	sleep(1);
	pthread_create(&t_chef, NULL, chef, NULL);

	pthread_join(t_customer1, NULL);
	pthread_join(t_customer2, NULL);
	pthread_join(t_chef, NULL);

	pthread_mutex_destroy(&kitchen_lock);
	pthread_cond_destroy(&ramen_cond);
	printf("[main]finished sales\n");
	return (0);
}
