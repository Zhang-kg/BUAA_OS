/* See COPYRIGHT for copyright information. */

#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"
#include "queue.h"
#include "trap.h"
#include "mmu.h" 

#define LOG2NENV	10
#define NENV		(1<<LOG2NENV)
#define ENVX(envid)	((envid) & (NENV - 1))
#define GET_ENV_ASID(envid) (((envid)>> 11)<<6)

// Values of env_status in struct Env
#define ENV_FREE	0
#define ENV_RUNNABLE		1
#define ENV_NOT_RUNNABLE	2
#define PTHREAD_FREE 			0
#define PTHREAD_RUNNABLE		1
#define PTHREAD_NOT_RUNNABLE	2

#define PTHREAD_MAX 8
#define PTHREAD_CANCEL_ENABLE 1		// for cancel state(default)
#define PTHREAD_CANCEL_DISABLE 0	// for cancel state
#define PTHREAD_CANCEL_ASYNCHRONOUS 0		// for cancel type
#define PTHREAD_CANCEL_DEFERRED 1		// for cancel type(default)

#define SEM_FREE		0
#define SEM_VALID		1

#define PTHREAD_CANCELED_EXIT	99
struct Pcb {
	struct Trapframe pcb_tf;
	pthread_t pthread_id;
	u_int pcb_status;
	LIST_ENTRY(Pcb) pcb_sched_link;
	int pcb_joined_count;
	struct Pcb * pcb_joined_thread_ptr;
	void ** pcb_join_value_ptr;
	
	u_int pcb_pri;
	void * pcb_exit_ptr;
	int pcb_cancelState;
	int pcb_cancelType;
	u_int pcb_canceled;
	u_int pcb_detach;
	int pcb_exit_value;
	u_int pcb_nop[11];
};

struct Env {
	//struct Trapframe env_tf;        // Saved registers
	LIST_ENTRY(Env) env_link;       // Free list
	u_int env_id;                   // Unique environment identifier
	u_int env_parent_id;            // env_id of this env's parent
	//u_int env_status;               // Status of the environment
	Pde  *env_pgdir;                // Kernel virtual address of page dir
	u_int env_cr3;
	//LIST_ENTRY(Env) env_sched_link;
	//u_int env_pri;
	// Lab 4 IPC
	u_int env_ipc_waiting_pthread_no;
	u_int env_ipc_value;            // data value sent to us 
	u_int env_ipc_from;             // envid of the sender  
	u_int env_ipc_recving;          // env is blocked receiving
	u_int env_ipc_dstva;		// va at which to map received page
	u_int env_ipc_perm;		// perm of page mapping received

	// Lab 4 fault handling
	u_int env_pgfault_handler;      // page fault state
	u_int env_xstacktop;            // top of exception stack

	// Lab 6 scheduler counts
	u_int env_runs;			// number of times been env_run'ed
	u_int env_pthread_count;
	u_int env_nop[496];                  // align to avoid mul instruction
	struct Pcb env_pthreads[8];
};

struct sem {
	u_int sem_envid;
	int sem_head_index;
	int sem_tail_index;
	char sem_name[16];
	int sem_value;
	int sem_status;
	int sem_shared;
	int sem_wait_count;
	struct Pcb * sem_wait_list[10];
};

LIST_HEAD(Env_list, Env);
LIST_HEAD(Pcb_list, Pcb);
extern struct Env *envs;		// All environments
extern struct Env *curenv;	        // the current env
extern struct Pcb *curpcb;
extern struct Env_list env_sched_list[2]; // runnable env list
extern struct Pcb_list pcb_sched_list[2];

void env_init(void);
int env_alloc(struct Env **e, u_int parent_id);
int thread_alloc(struct Env * e, struct Pcb ** p);
void env_free(struct Env *);
void env_create_priority(u_char *binary, int size, int priority);
void env_create(u_char *binary, int size);
void env_destroy(struct Env *e);

int envid2env(u_int envid, struct Env **penv, int checkperm);
int pthreadid2pcb(u_int pthreadid, struct Pcb ** ppcb);
void env_run(struct Pcb *p);
void thread_destroy(struct Pcb * p);
void thread_free(struct Pcb * p);
int thread_alloc(struct Env * e, struct Pcb ** new);
int pthreadid2pcb(u_int threadid, struct Pcb ** ppcb);
u_int mkpcbid(struct Pcb * p);


// for the grading script
#define ENV_CREATE2(x, y) \
{ \
	extern u_char x[], y[]; \
	env_create(x, (int)y); \
}
#define ENV_CREATE_PRIORITY(x, y) \
{\
        extern u_char binary_##x##_start[]; \
        extern u_int binary_##x##_size;\
        env_create_priority(binary_##x##_start, \
                (u_int)binary_##x##_size, y);\
}
#define ENV_CREATE(x) \
{ \
	extern u_char binary_##x##_start[];\
	extern u_int binary_##x##_size; \
	env_create(binary_##x##_start, \
		(u_int)binary_##x##_size); \
}

#endif // !_ENV_H_

