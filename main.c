#include <stdio.h>
#include <pthread.h>
#include "codexion.h"
#include <time.h>
#include <sys/time.h>

int	main(int argc, char **argv)
{
  struct s_Args args;
	struct s_SharedContext shared_ctx;

  if (parse_args(argc, argv, &args) != 0)
    return -1;
  if (init_context(&args, &shared_ctx) != 0)
    return -1;
  run_simulation(&shared_ctx);
  cleanup_context(&shared_ctx);
  printf("argc:%d, argv:%s\n", argc, argv[1]);

	return (0);
}
