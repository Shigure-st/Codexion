#include <stdio.h>
#include <pthread.h>
#include "codexion.h"

int	main(int argc, char **argv)
{
  struct s_Args args;
	struct s_SharedContext shared_ctx;

  if (parse_args(argc, argv, &args) != 0)
    return -1;
  init_context(&args, &shared_ctx);
  run_simulation(&shared_ctx);
  printf("argc:%d, argv:%s\n", argc, argv[1]);
  pthread_cond_init(&shared_ctx.cond, NULL);

	return (0);
}
