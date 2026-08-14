#include <stdio.h>
#include <pthread.h>
#include "codexion.h"
#include <time.h>
#include <sys/time.h>

int	main(int argc, char **argv)
{
  // struct timeval tv;
  // struct timespec ts;
  // gettimeofday(&tv, NULL);
  // int i = 10000;
  // long  total_usec;
  // long  temp_usec;
  // printf("秒:%ld\n", (long)tv.tv_sec);
  // printf("マイクロ秒:%ld\n", (long)tv.tv_usec);
  // total_usec = tv.tv_usec + (i * 1000);
  // ts.tv_sec = tv.tv_sec + (total_usec / 1000000);
  // temp_usec = total_usec % 1000000;
  // ts.tv_nsec = temp_usec * 1000;
  //
  // printf("秒:%ld\n", (long)ts.tv_sec);
  // printf("ナノ秒:%ld\n", (long)ts.tv_nsec);
  //
  // return 0;
  struct s_Args args;
	struct s_SharedContext shared_ctx;

  if (parse_args(argc, argv, &args) != 0)
    return -1;
  if (init_context(&args, &shared_ctx) != 0)
    return -1;
  run_simulation(&shared_ctx);
  cleanup_context(&shared_ctx);
  printf("argc:%d, argv:%s\n", argc, argv[1]);
  // pthread_cond_init(&shared_ctx.cond, NULL);

	return (0);
}
