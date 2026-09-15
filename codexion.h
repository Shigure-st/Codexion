/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:44:22 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:44:24 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <sys/time.h>

typedef struct s_args			t_args;
typedef struct s_shared_context	t_shared_context;
typedef struct s_coder			t_coder;
typedef struct s_dongle			t_dongle;
typedef struct s_monitor		t_monitor;
typedef struct s_heap			t_heap;
typedef struct s_heap_data		t_heap_data;

struct s_args
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

struct s_heap
{
	int			size;
	int			capa;
	t_heap_data	*data;
};

struct s_heap_data
{
	t_coder		*coder;
	long long	priority;
};

struct s_shared_context
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
	t_dongle		*dongles;
	t_coder			*coders;
	t_monitor		*mon;
	long long		next_seq;
	long long		start_time_ms;
	pthread_mutex_t	lock;
	pthread_mutex_t	log_lock;
};

struct s_dongle
{
	int				i;
	bool			free;
	t_heap			*waiters;
	long long		t_end;
	pthread_cond_t	*cond;
	pthread_mutex_t	lock;
	struct timespec	ts;
};

struct s_coder
{
	int					id;
	bool				is_comp;
	bool				wait;
	bool				done;
	long long			t_last;
	pthread_t			th;
	pthread_cond_t		cond;
	pthread_mutex_t		lock;
	t_dongle			*r_dongle;
	t_dongle			*l_dongle;
	t_dongle			*first;
	t_dongle			*second;
	t_monitor			*mon;
	t_shared_context	*ctx;
	struct timeval		tv;
	struct timespec		ts;
};

struct s_monitor
{
	pthread_t			th;
	long long			w_time;
	pthread_cond_t		cond;
	pthread_mutex_t		lock;
	t_shared_context	*ctx;
};

int				main(int argc, char **argv);
int				is_debug(t_coder *coder);
int				is_refactor(t_coder *coder);
int				is_compile(t_coder *coder);
int				heap_pop(t_dongle *dongle);
int				alloc_heapqueue(t_shared_context *shared_ctx);
int				parse_char(char *arg, char **target);
int				parse_int(char *arg, int *target);
int				parse_args(int argc, char **argv, t_args *args);
int				alloc_dongle_array(t_shared_context *shared_ctx);
int				alloc_coder_array(t_shared_context *shared_ctx);
int				alloc_monitor(t_shared_context *shared_ctx);
int				init_context(t_args *args, t_shared_context *shared_ctx);
int				init_coder_mutex(t_shared_context *shared_ctx);
int				run_simulation(t_shared_context *shared_ctx);
int				cleanup_context(t_shared_context *shared_ctx);
int				get_min_child(t_heap *queue, int left, int right);
bool			is_empty_and_free(t_dongle *dongle);
bool			try_to_acquire(t_coder *coder);
bool			check_complete(t_shared_context *shared_ctx);
bool			is_stopped(t_shared_context *ctx);
bool			update_last_compile_time(t_coder *coder);
bool			handle_single_coder(t_coder *coder);
bool			is_expired(t_coder *coder);
bool			is_done(t_coder *coder);
void			*check_burnout(void *arg);
void			*simulate(void *arg);
void			heap_push(t_dongle *dongle, t_coder *coder);
void			acquire_dongles(t_coder *coder);
void			wakeup_all_thread(t_shared_context *shared_ctx, int coder);
void			free_dongle_heap(t_dongle *d);
void			set_stop_flag(t_shared_context *ctx);
void			output_log(t_shared_context *ctx, int id, const char *message);
void			shift_up(t_heap *queue);
void			shift_down(t_heap *queue);
void			set_coder_sleep(t_coder *coder, int wait_ms);
void			take_dongles(t_coder *coder);
void			release_dongles(t_coder *coder);
void			wait_for_dongles(t_coder *coder);
void			order_by_address(t_coder *coder);
long long		get_time_in_ms(void);
long long		get_last_compile_time(t_coder *coder);
struct timespec	wakeup_time(t_coder *coder);
struct timespec	ms_to_timespec(long long ms);

#endif
