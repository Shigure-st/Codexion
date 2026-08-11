#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codexion.h"


int parse_char(char *arg, char **target)
{
  if(strcmp(arg, "fifo") == 0
     || strcmp(arg, "edf") == 0)
    return (*target = arg, 0);
  return 1;
}

int parse_int(char *arg, int *target)
{
  int val;

  val = atoi(arg);
  if(val != 0)
    return (*target = val, 0);
  return 1;
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
