#ifndef _INC_PTHREAD_H_
#define _INC_PTHREAD_H_

#include "types.h"
#include "queue.h"
#include "trap.h"
#include "mmu.h" 
#include "types.h"

// pthread - status
#define PTHREAD_FREE 			0
#define PTHREAD_RUNNABLE		1
#define PTHREAD_NOT_RUNNABLE	2
// pthread - limit
#define PTHREAD_MAX  16
// pthread - stack size
#define PTHREAD_MAX_STACKSIZE   PDMAP
// cancel state and types
#define PTHREAD_CANCEL_ENABLE 1		// for cancel state(default)
#define PTHREAD_CANCEL_DISABLE 0	// for cancel state
#define PTHREAD_CANCEL_ASYNCHRONOUS 0		// for cancel type
#define PTHREAD_CANCEL_DEFERRED 1		// for cancel type(default)
// sem_status
#define SEM_FREE		0
#define SEM_VALID		1
// exit
#define PTHREAD_CANCELED_EXIT	99
// join
#define PTHREAD_CREATE_JOINABLE	    0
#define PTHREAD_CREATE_DETACHED		1
// attr - sched
#define SCHED_OTHER 0
#define SCHED_FIFO	1
#define SCHED_RR	2
// attr pri
#define PTHREAD_MAX_PRIORITY    40
#define PTHREAD_MIN_PRIORITY    1
#define PTHREAD_MAX_RTPRIORITY  99
#define PTHREAD_MIN_RTPRIORITY  1
#define PTHREAD_BASE_RTOFFSET   1000

#define PTHREAD_SCHED_WEIGHT(pcb)   ((pcb -> attr.schedpolocy == SCHED_OTHER) ? \
                (pcb) -> attr.schedpriority : (pcb) -> attr.schedrtpriority + PTHREAD_BASE_RTOFFSET)

struct pthread_attr {
	int detachstate;
	int schedpolocy;
	int schedpriority;
	int schedrtpriority;
	int counter;
	int stacksize;
};

struct Pcb {
	struct Trapframe pcb_tf;
	pthread_t pthread_id;
	u_int pcb_status;
	LIST_ENTRY(Pcb) pcb_sched_link;

	int pcb_joined_count;
	struct Pcb * pcb_joined_thread_ptr;
	void ** pcb_join_value_ptr;

	int pcb_exit_value;
	void * pcb_exit_ptr;

	int pcb_cancelState;
	int pcb_cancelType;
	u_int pcb_canceled;

	struct pthread_attr attr;
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



struct sched_param {
    int sched_priority;
    int sched_rtpriority;
};
#endif
