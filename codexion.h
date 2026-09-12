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

// struct s_Boss
// {
// 	pthread_t	      t_Boss;
//   pthread_mutex_t request_mutex;
//   bool            request_flag;
// 	struct s_SharedContext	*shared_ctx;
// };

struct s_SharedContext
{
  int			coder;
  int			burnout;
  int			debug;
  int			refactor;
  int			compile;
  int     required;
  int     cooldown;
  long long next_seq;
  char  *scheduler;
  bool		is_active;
  bool		stop_flag;
	t_Dongle	*dongles;
	t_Coder		*coders;
  t_Queue  *queue;
  // t_Boss    *boss;
  t_Monitor    *mon;
  pthread_cond_t  cond;
};

struct s_Dongle
{
	int				i;
	pthread_mutex_t lock;
	bool			free;
  long long t_end;
  pthread_cond_t  *cond;
  struct timespec  ts;
  t_Heap *waiters;
};

struct s_Coder
{
	int				id;
	bool			is_comp;
  bool      wait;
  bool      done;
  long      t_last;
	pthread_t	th;
  pthread_cond_t  cond;
	pthread_mutex_t lock;
  struct timeval  tv;
  struct timespec  ts;
	struct s_Dongle	*r_dongle;
	struct s_Dongle	*l_dongle;
  struct s_Monitor *mon;
  // struct s_Boss *boss;
	struct s_SharedContext	*ctx;
};

struct s_Monitor
{
	pthread_t	      th;
  pthread_mutex_t lock;
  struct timeval  tv;
	struct s_SharedContext	*ctx;
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
int heap_pop(t_Dongle *dongle);
void heap_push(t_Dongle *dongle, t_Coder *coder);
bool  is_empty_and_free(t_Dongle *dongle);
int alloc_heapqueue(t_SharedContext *shared_ctx);
void free_dongle_heap(t_Dongle *d);
void acquire_dongles(t_Coder *coder);
struct timespec wakeup_time(t_Coder *coder);
bool try_to_acquire(t_Coder *coder);
bool  check_complete(t_SharedContext *shared_ctx);

#endif
