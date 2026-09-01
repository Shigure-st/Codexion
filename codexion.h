#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <time.h>
# include <sys/time.h>


typedef struct s_Args t_Args;
typedef struct s_SharedContext t_SharedContext;
typedef struct s_Coder t_Coder;
typedef struct s_Dongle t_Dongle;
typedef struct s_Monitor t_Monitor;
typedef struct s_Boss t_Boss;
typedef struct s_Data t_Data;
typedef struct s_Queue t_Queue;
typedef struct s_Heap t_Heap;
typedef struct s_HeapData t_HeapData;

struct s_Args
{
  int   coder;
  int   burnout;
  int   compile;
  int   debug;
  int   refactor;
  int   required;
  int   cooldown;
  char  *scheduler;
};

struct s_Heap
{
  t_HeapData    *data;
  int           size;
  int           capa;
};

struct s_HeapData
{
  t_Coder *coder;
  long long  priority;
};

struct s_Queue
{
  struct s_Data *arr;
  int           tail;
  int           head;
  int           size;
  pthread_cond_t  not_empty;
};

struct s_Data
{
  struct s_Coder  *coder;
  int  priority;
};

struct s_Boss
{
	pthread_t	      t_Boss;
  pthread_mutex_t request_mutex;
  bool            request_flag;
	struct s_SharedContext	*shared_ctx;
};

struct s_SharedContext
{
  int			coder;
  int			burnout;
  int			debug;
  int			refactor;
  int			compile;
  int     required;
  int     cooldown;
  char  *scheduler;
  bool		is_active;
  bool		stop_flag;
	t_Dongle	*dongles;
	t_Coder		*coders;
  t_Queue  *queue;
  t_Boss    *boss;
  t_Monitor    *monitor;
  pthread_cond_t  cond;
};

struct s_Dongle
{
	int				i;
	pthread_mutex_t dongle_lock;
	bool			available;
  long long cooldown_end_time;
  struct timespec  ts;
  t_Heap *wait_coders;
};

struct s_Coder
{
	int				number;
	bool			is_compile;
  bool      wait_cond;
  bool      is_complete;
  long      last_compile_time;
	pthread_t	t_Coder;
  pthread_cond_t  check_compile_cond;
	pthread_mutex_t local_mutex;
  struct timeval  tv;
  struct timespec  ts;
	struct s_Dongle	*right_dongle;
	struct s_Dongle	*left_dongle;
  struct s_Monitor *monitor;
  struct s_Boss *boss;
	struct s_SharedContext	*shared_ctx;
};

struct s_Monitor
{
	pthread_t	      t_Monitor;
  pthread_mutex_t burnout_mutex;
  struct timeval  tv;
	struct s_SharedContext	*shared_ctx;
};


bool  is_queue_empty(t_Queue *queue);
void  enqueue(t_Queue *queue, t_Coder *element);
void  *dequeue(t_Queue *queue);
int	is_debug(t_Coder *coder);
int	  is_refactor(t_Coder *coder);
int	is_compile(t_Coder *coder);
void  *receive_from_coder(void* arg);
void	*simulate(void* arg);
int parse_char(char *arg, char **target);
int parse_int(char *arg, int *target);
int parse_args(int argc, char **argv, t_Args *args);
int alloc_dongle_array(t_SharedContext *shared_ctx);
int alloc_coder_array(t_SharedContext *shared_ctx);
int alloc_queue(t_SharedContext *shared_ctx);
int alloc_boss(t_SharedContext *shared_ctx);
int alloc_monitor(t_SharedContext *shared_ctx);
int init_context(t_Args *args, t_SharedContext *shared_ctx);
int init_coder_mutex(t_SharedContext *shared_ctx);
void wakeup_all_thread(t_SharedContext *shared_ctx, int coder);
int run_simulation(t_SharedContext *shared_ctx);
int	main(int argc, char **argv);
int  cleanup_context(t_SharedContext *shared_ctx);
void  *check_burnout(void* arg);
long long  get_time_in_ms(void);
void  free_heapqueue(t_SharedContext *shared_ctx);
int heap_pop(t_Heap *queue, t_HeapData *ret);
void heap_push(t_Heap *queue, t_HeapData *push_data);

#endif
