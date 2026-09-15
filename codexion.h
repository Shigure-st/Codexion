#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <sys/time.h>

typedef struct s_Args			t_Args;
typedef struct s_SharedContext	t_SharedContext;
typedef struct s_Coder			t_Coder;
typedef struct s_Dongle			t_Dongle;
typedef struct s_Monitor		t_Monitor;
typedef struct s_Heap			t_Heap;
typedef struct s_HeapData		t_HeapData;

struct s_Args
{
	int		coder;
	int		burnout;
	int		compile;
	int		debug;
	int		refactor;
	int		required;
	int		cooldown;
	char	*scheduler;
};

struct s_Heap
{
	int			size;
	int			capa;
	t_HeapData	*data;
};

struct s_HeapData
{
	t_Coder		*coder;
	long long	priority;
};

struct s_SharedContext
{
	int				coder;
	int				burnout;
	int				debug;
	int				refactor;
	int				compile;
	int				required;
	int				cooldown;
	char			*scheduler;
	bool			is_active;
	bool			stop_flag;
	bool			is_lock;
	bool			is_log_lock;
	bool			is_cond;
	t_Dongle		*dongles;
	t_Coder			*coders;
	t_Monitor		*mon;
	long long		next_seq;
	long long		start_time_ms;
	pthread_mutex_t	lock;
	pthread_mutex_t	log_lock;
};

struct s_Dongle
{
	int				i;
	bool			free;
	t_Heap			*waiters;
	long long		t_end;
	pthread_cond_t	*cond;
	pthread_mutex_t	lock;
	struct timespec	ts;
};

struct s_Coder
{
	int				id;
	bool			is_comp;
	bool			wait;
	bool			done;
	long long		t_last;
	pthread_t		th;
	pthread_cond_t	cond;
	pthread_mutex_t	lock;
	t_Dongle		*r_dongle;
	t_Dongle		*l_dongle;
	t_Dongle		*first;
	t_Dongle		*second;
	t_Monitor		*mon;
	t_SharedContext	*ctx;
	struct timeval	tv;
	struct timespec	ts;
};

struct s_Monitor
{
	pthread_t		th;
	long long		w_time;
	pthread_cond_t	cond;
	pthread_mutex_t	lock;
	t_SharedContext	*ctx;
};

int				main(int argc, char **argv);
int				is_debug(t_Coder *coder);
int				is_refactor(t_Coder *coder);
int				is_compile(t_Coder *coder);
int				heap_pop(t_Dongle *dongle);
int				alloc_heapqueue(t_SharedContext *shared_ctx);
int				parse_char(char *arg, char **target);
int				parse_int(char *arg, int *target);
int				parse_args(int argc, char **argv, t_Args *args);
int				alloc_dongle_array(t_SharedContext *shared_ctx);
int				alloc_coder_array(t_SharedContext *shared_ctx);
int				alloc_monitor(t_SharedContext *shared_ctx);
int				init_context(t_Args *args, t_SharedContext *shared_ctx);
int				init_coder_mutex(t_SharedContext *shared_ctx);
int				run_simulation(t_SharedContext *shared_ctx);
int				cleanup_context(t_SharedContext *shared_ctx);
int				get_min_child(t_Heap *queue, int left, int right);
bool			is_empty_and_free(t_Dongle *dongle);
bool			try_to_acquire(t_Coder *coder);
bool			check_complete(t_SharedContext *shared_ctx);
bool			is_stopped(t_SharedContext *ctx);
bool			update_last_compile_time(t_Coder *coder);
bool			handle_single_coder(t_Coder *coder);
bool			is_expired(t_Coder *coder);
void			*check_burnout(void *arg);
void			*simulate(void *arg);
void			heap_push(t_Dongle *dongle, t_Coder *coder);
void			acquire_dongles(t_Coder *coder);
void			wakeup_all_thread(t_SharedContext *shared_ctx, int coder);
void			free_dongle_heap(t_Dongle *d);
void			set_stop_flag(t_SharedContext *ctx);
void			output_log(t_SharedContext *ctx, int id, const char *message);
void			shift_up(t_Heap *queue);
void			shift_down(t_Heap *queue);
void			set_coder_sleep(t_Coder *coder, int wait_ms);
void			take_dongles(t_Coder *coder);
void			release_dongles(t_Coder *coder);
void			wait_for_dongles(t_Coder *coder);
void			order_by_address(t_Coder *coder);
long long		get_time_in_ms(void);
long long		get_last_compile_time(t_Coder *coder);
struct timespec	wakeup_time(t_Coder *coder);
struct timespec	ms_to_timespec(long long ms);

#endif
