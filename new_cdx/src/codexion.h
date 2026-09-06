/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:24:55 by mennih            #+#    #+#             */
/*   Updated: 2026/09/06 18:29:31 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdbool.h>
# include <limits.h>

typedef enum e_scheduler
{
	POLICY_FIFO,
	POLICY_EDF
}	t_scheduler;

typedef struct s_sim		t_sim;
typedef struct s_dongle		t_dongle;
typedef struct s_coder		t_coder;

/*
** One entry in the sim-wide wait-heap: a coder's request for its OWN
** pair of dongles (sim->coders[coder_id - 1].left/right), ranked by
** priority (arrival ticket for fifo, deadline for edf).
*/
typedef struct s_request
{
	long long	priority;
	int			coder_id;
}	t_request;

struct s_dongle
{
	int			id;
	bool		in_use;
	long long	release_time;
};

struct s_coder
{
	int				id;
	t_dongle		*left;
	t_dongle		*right;
	/* last_compile_start and compile_count are read by the monitor
	** thread and written by this coder's own thread: both accesses
	** must go through cond_mutex. */
	long long		last_compile_start;
	int				compile_count;
	/* ticket is this coder's arrival priority for the CURRENT attempt,
	** captured once before requesting dongles, so the whole combined
	** request is ranked as a single event. */
	long long		ticket;
	/* Set by the arbiter (dispatch), under cond_mutex, the instant this
	** coder's combined request is granted. This coder waits on exactly
	** this flag, guarded by the same mutex used to wait on it. */
	bool			granted;
	bool			burned_out;
	t_sim			*sim;
	pthread_cond_t	cond;
	pthread_mutex_t	cond_mutex;
};

struct s_sim
{
	int				n;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				compiles_required;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
	t_coder			*coders;
	t_dongle		*dongles;
	long long		start_time_ms;
	volatile bool	stopped;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	/* The central arbiter: every "I need my two dongles" request from
	** every coder goes through this ONE heap, guarded by ONE mutex, so
	** a grant is always an atomic both-or-neither decision. */
	pthread_mutex_t	arb_mutex;
	t_request		*heap;
	int				heap_size;
	int				heap_cap;
	/* Scratch space dispatch() uses to set aside requests that are not
	** yet grantable while it keeps scanning the rest in priority
	** order; sized once at init, capacity == n (the most that can
	** ever be pending at once). */
	t_request		*pending;
	pthread_t		*threads;
	pthread_t		monitor_thread;
};

long long	get_time_ms(void);
long long	sim_elapsed(t_sim *sim);
void		log_event(t_sim *sim, int coder_id, const char *msg);
void		log_burnout(t_sim *sim, int coder_id);
int			msleep(long long ms);
bool		sim_is_stopped(t_sim *sim);
void		sim_stop(t_sim *sim);

bool		is_valid_uint(const char *s);
long long	safe_atoll(const char *s);

int			req_less(const t_request *a, const t_request *b);
void		heap_swap(t_request *heap, int i, int j);
void		heap_sift_up(t_request *heap, int idx);
void		heap_sift_down(t_request *heap, int size, int idx);
int			heap_push(t_sim *sim, t_request req);
void		heap_remove(t_sim *sim, int idx);
int			heap_find(t_sim *sim, int coder_id);

void		dongle_init(t_dongle *d, int id);
bool		dongle_ready(t_dongle *d, long long cooldown_ms);
void		dispatch(t_sim *sim);
void		dongle_request(t_sim *sim, t_coder *c);
void		dongle_release(t_sim *sim, t_coder *c);
void		dongle_wait(t_coder *c);
bool		coder_wait_grant(t_sim *sim, t_coder *c);

void		*coder_routine(void *arg);
void		put_dongles(t_coder *c);
void		*monitor_routine(void *arg);

int			parse_args(int argc, char **argv, t_sim *sim);
int			sim_init(t_sim *sim);
void		sim_cleanup(t_sim *sim);
int			sim_run(t_sim *sim);

#endif