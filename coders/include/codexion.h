#ifndef CODEXION_H
# define CODEXION_H

# define DONGLE_QUEUE_CAPACITY 2
#define MSG_DONGLE "has taken a dongle"
#define MSG_COMPILE "is compiling"
#define MSG_DEBUG "is debugging"
#define MSG_REFACTOR "is refactoring"
#define MSG_BURNOUT "burned out"

# include <pthread.h>

typedef struct s_simulation	t_simulation;

typedef enum e_scheduler
{
	POLICY_FIFO,
	POLICY_EDF
}	t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_request
{
	int				coder_id;
	unsigned long	sequence;
	long long		deadline;
}	t_request;

typedef struct s_heap
{
	t_request	items[DONGLE_QUEUE_CAPACITY];
	int			size;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				owner_id;
	long long		cooldown_until;
	t_heap			queue;
	pthread_mutex_t	mutex;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				left_dongle;
	int				right_dongle;
	int				compiles_done;
	long long		last_compile_start;
	pthread_t		thread;
	t_simulation	*simulation;
}	t_coder;

struct s_simulation
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	long long		start_time;
	int				started;
	int				stopped;
	unsigned long	request_sequence;
	pthread_t		monitor_thread;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	wait_mutex;
	pthread_cond_t	wait_condition;
};


int	parse_arguments(int argc, char **argv, t_config *config);
long long	get_time_ms(void);
int		init_sync(t_simulation *simulation);
void	destroy_sync(t_simulation *simulation);
int		init_simulation(t_simulation *simulation, t_config *config);
void	destroy_simulation(t_simulation *simulation);
int	request_has_priority(const t_request *first,
		const t_request *second, t_scheduler scheduler);
int	heap_push(t_heap *heap, t_request request, t_scheduler scheduler);
t_request	*heap_peek(t_heap *heap);
int			heap_pop(t_heap *heap);
void	create_request(t_coder *coder, t_request *request);
int		enqueue_request(t_coder *coder, t_request request);
void	lock_dongles(t_dongle *left, t_dongle *right);
void	unlock_dongles(t_dongle *left, t_dongle *right);
int	request_is_top(t_heap *heap, t_request request);
int	request_can_acquire(t_coder *coder, t_request request);
int	wait_for_dongles(t_coder *coder, t_request request);
int		simulation_is_stopped(t_simulation *simulation);
void	stop_simulation(t_simulation *simulation);
int		try_acquire_dongles_locked(t_coder *coder,
			t_request request, long long *cooldown);
void	release_dongles(t_coder *coder);
void	log_state(t_coder *coder, const char *message);
int	sleep_ms(t_simulation *simulation, int duration);
void	*coder_routine(void *argument);
int		wait_for_start(t_simulation *simulation);
void	begin_simulation(t_simulation *simulation);
int		create_coder_threads(t_simulation *simulation);
void	join_coder_threads(t_simulation *simulation);
void	*monitor_routine(void *argument);

#endif