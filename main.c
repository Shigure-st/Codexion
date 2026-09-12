#include <stdio.h>
#include <pthread.h>
#include "codexion.h"
#include <time.h>
#include <sys/time.h>

int	main(int argc, char **argv)
{
  struct s_Args args;
	struct s_SharedContext ctx;

  if (parse_args(argc, argv, &args) != 0)
    return -1;
  if (init_context(&args, &ctx) != 0)
    return -1;
  run_simulation(&ctx);
  cleanup_context(&ctx);
  printf("argc:%d, argv:%s\n", argc, argv[1]);
  // pthread_cond_init(&shared_ctx.cond, NULL);

	return (0);
}
