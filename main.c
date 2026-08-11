#include <stdio.h>
#include <pthread.h>
#include "codexion.h"

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
