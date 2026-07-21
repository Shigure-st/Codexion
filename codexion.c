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

struct s_SharedContext
{
	int			number_of_coders;
	int			time_to_debug;
	int			time_to_refactor;
	int			time_to_compile;
	bool		is_active;
	t_Dongle	*dongle_array;
	t_Coder		*coder_array;
};

struct s_Dongle
{
	pthread_mutex_t dongle_lock;
};

struct s_Coder
{
	int				number;
	struct s_Dongle	right_hand_dongle;
	struct s_Dongle	left_hand_dongle;
	struct s_SharedContext	*shared_ctx;
};

// void *customer(void* arg)
// {
// 	char	*name = (char*)arg;
//
// 	pthread_mutex_lock(&kitchen_lock);
// 	while (ramen_ready == 0)
// 	{
// 		printf("[%s] waiting for ramen... \n", name);
// 		// pthread_cond_wait(&ramen_cond, &kitchen_lock);
// 	}
// 	printf("[%s] oh, coming ramen\n", name);
//
// 	pthread_mutex_unlock(&kitchen_lock);
// 	return NULL;
// }
//
// void *chef(void* arg)
// {
// 	printf("[staff] I start to make ramen....\n");
// 	sleep(5);
//
// 	pthread_mutex_lock(&kitchen_lock);
// 	ramen_ready = 1;
// 	printf("[staff] welcome ramen\n");
// 	pthread_cond_broadcast(&ramen_cond);
// 	pthread_mutex_unlock(&kitchen_lock);
// 	return NULL;
// }
//
void	is_debug(void)
{
  usleep(3000);
	printf("Now Debug....\n");
}

void	is_refactor(void)
{
  usleep(3000);
	printf("Now Refactoring....\n");
}

void	is_compile(t_Dongle right_hand_dongle, t_Dongle left_hand_dongle, int number)
{
  pthread_mutex_lock(&right_hand_dongle.dongle_lock);
  pthread_mutex_lock(&left_hand_dongle.dongle_lock);
  printf("Coder Number:%d", number);
  usleep(10000);
  printf("Now Compile....\n");
 	pthread_mutex_unlock(&right_hand_dongle.dongle_lock);
 	pthread_mutex_unlock(&left_hand_dongle.dongle_lock);
  is_debug();
  is_refactor();

}


void	*simulate(void* arg)
{
	printf("Coder Number:%d\n", ((struct s_Coder *)arg)->number);
	is_compile(((struct s_Coder *)arg)->right_hand_dongle, ((struct s_Coder *)arg)->right_hand_dongle, ((struct s_Coder *)arg)->number);
	// is_debug();
	// is_refactor();

	return NULL;
}

int	main()
{
	struct s_SharedContext shared_ctx;
	struct s_Dongle dongle_array[2];
	struct s_Coder coder_array[2];

  pthread_mutex_init(&dongle_array[0].dongle_lock, NULL);
  pthread_mutex_init(&dongle_array[1].dongle_lock, NULL);

	shared_ctx.number_of_coders = 2;
	shared_ctx.time_to_debug = 200;
	shared_ctx.time_to_refactor = 200;
	shared_ctx.time_to_compile = 200;
	shared_ctx.is_active = true;

	coder_array[0].number = 1;
	coder_array[0].left_hand_dongle = dongle_array[0];
	coder_array[0].right_hand_dongle = dongle_array[1];
	coder_array[0].shared_ctx = &shared_ctx;

	coder_array[1].number = 2;
	coder_array[1].right_hand_dongle = dongle_array[0];
	coder_array[1].left_hand_dongle = dongle_array[1];
	coder_array[1].shared_ctx = &shared_ctx;

	// printf("Nnmber_of_coders:%d\n", shared_ctx.number_of_coders);
	// printf("coder_array[0]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[0].number, coder_array[0].left_hand_dongle, coder_array[0].right_hand_dongle);
	// printf("coder_array[0] shared_ctx.time_to_debug %d\n", coder_array[0].shared_ctx->time_to_debug);
	// printf("coder_array[1]:number %d, left_hand_dongle %p, right_hand_dongle %p\n", coder_array[1].number, coder_array[1].left_hand_dongle, coder_array[1].right_hand_dongle);

	pthread_t	t_coder1;
	pthread_t	t_coder2;

	pthread_create(&t_coder1, NULL, simulate, &coder_array[0]);
	pthread_create(&t_coder2, NULL, simulate, &coder_array[1]);

	pthread_join(t_coder1, NULL);
	pthread_join(t_coder2, NULL);
	// pthread_t	t_chef, t_customer1, t_customer2;
	//
	// pthread_mutex_init(&kitchen_lock, NULL);
	// pthread_cond_init(&ramen_cond, NULL);
	//
	// pthread_create(&t_customer1, NULL, customer, "Kyaku:A");
	// pthread_create(&t_customer2, NULL, customer, "Kyaku:B");
	// sleep(1);
	// pthread_create(&t_chef, NULL, chef, NULL);
	//
	// pthread_join(t_customer1, NULL);
	// pthread_join(t_customer2, NULL);
	// pthread_join(t_chef, NULL);
	//
	// pthread_mutex_destroy(&kitchen_lock);
	// pthread_cond_destroy(&ramen_cond);
	// printf("[main]finished sales\n");
	return (0);
}
