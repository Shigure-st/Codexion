#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codexion.h"
#include <limits.h>


int parse_char(char *arg, char **target)
{
  if(strcmp(arg, "fifo") == 0
     || strcmp(arg, "edf") == 0)
    return (*target = arg, 0);
  return 1;
}

int check_numeric(char *arg)
{
  int i;

  if (arg == NULL || arg[0] == '\0')
      return 0;
  i = 0;
  while (arg[i] != '\0')
  {
    if (arg[i] < '0' || arg[i] > '9')
      return 0;
    i++;
  }
  return 1;
}

int parse_int(char *arg, int *target)
{
  long long	num;
	int			i;

  if (!check_numeric(arg))
    return 1;
	num = 0;
	i = 0;
  if (arg[0] == '0' && arg[i + 1] != '\0')
    return 1;
	while (arg[i] != '\0')
	{
		num = num * 10 + (arg[i] - '0');
		if (num > INT_MAX)
			return 1;
		i++;
	}
	*target = (int)num;
	return 0;
}

int parse_args(int argc, char **argv, t_Args *args)
{

  if(argc != 9)
    return (fprintf(stderr, "Error:Invalid arguments\n"), -1);

  if(parse_int(argv[1], &(args->coder)) != 0
     || parse_int(argv[2], &(args->burnout)) != 0
     || parse_int(argv[3], &(args->compile)) != 0
     || parse_int(argv[4], &(args->debug)) != 0
     || parse_int(argv[5], &(args->refactor)) != 0
     || parse_int(argv[6], &(args->required)) != 0
     || parse_int(argv[7], &(args->cooldown)) != 0)
    return (fprintf(stderr, "Error:Invalid integer arguments\n"), -1);
  if(parse_char(argv[8], &(args->scheduler)) != 0)
    return (fprintf(stderr, "Error:Invalid string arguments\n"), -1);
  if (args->coder == 0)
    return (fprintf(stderr, "Error:Invalid Coder arguments\n"), -1);

  // printf("arg1:%d\n", args->coder);
  // printf("arg2:%d\n", args->burnout);
  // printf("arg3:%d\n", args->compile);
  // printf("arg4:%d\n", args->debug);
  // printf("arg5:%d\n", args->refactor);
  // printf("arg6:%d\n", args->required);
  // printf("arg7:%d\n", args->cooldown);
  // printf("arg8:%s\n", args->scheduler);

  return 0;
}
