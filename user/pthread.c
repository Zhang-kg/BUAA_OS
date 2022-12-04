#include "lib.h"
#include <error.h>
#include <mmu.h>


int pthread_setstacksize(pthread_t thread, int stacksize) {
	struct Pcb * p;
	
	p = &env -> env_pthreads[thread & 0xf];
	if (p -> pthread_id != thread || p -> pcb_status == PTHREAD_FREE) {
		return -E_INVAL;
	}

	if (p -> attr.stacksize > stacksize) {
		return -E_INVAL;
	}
	p -> attr.stacksize = stacksize;
	return 0;
}

int pthread_self(void) {
	return syscall_get_threadid();
}

int pthread_setschedprio(pthread_t thread, int prio) {
	struct Pcb * p;
	
	p = &env -> env_pthreads[thread & 0xf];
	if (p -> pthread_id != thread || p -> pcb_status == PTHREAD_FREE) {
		return -E_INVAL;
	}
	
	if (prio >= PTHREAD_MIN_PRIORITY && prio <= PTHREAD_MAX_PRIORITY) {
		p -> attr.schedpriority = prio;
		return 0;
	} else return -E_INVAL;
}

int pthread_getschedprio(pthread_t thread, int * prio) {
	struct Pcb * p;
	p = &env -> env_pthreads[thread & 0xf];
	if (p -> pthread_id != thread || p -> pcb_status == PTHREAD_FREE) {
		return -E_INVAL;
	}
	if (prio == NULL) return -E_INVAL;
	*prio = p -> attr.schedpriority;
	return 0;
}

int pthread_sched_setscheduler(pthread_t thread, int policy, const struct sched_param * param) {
	struct Pcb * p;
	p = &env -> env_pthreads[thread & 0xf];
	if (p -> pthread_id != thread || p -> pcb_status == PTHREAD_FREE) {
		return -E_INVAL;
	}
	p -> attr.schedpriority = 1;
	p -> attr.schedrtpriority = 1;
	if (param == NULL) return -E_INVAL;
	if (policy == SCHED_OTHER) {
		pthread_setschedprio(p -> pthread_id, param -> sched_priority);
	} else {
		if (param -> sched_rtpriority >= PTHREAD_MIN_RTPRIORITY && param -> sched_rtpriority <= PTHREAD_MAX_PRIORITY) {
			p -> attr.schedrtpriority = param -> sched_rtpriority;
		} else return -E_INVAL;
	}
	return 0;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void * (*start_rountine)(void *), void *arg) {
	int newthread = syscall_thread_alloc();
	writef("finish thread alloc in user/pthread.c/pthread_create\n");
	if (newthread < 0) {
		*thread = 0;
		return newthread;
	}
	struct Pcb *p = &env->env_pthreads[newthread];
	//p->pcb_tf.regs[29] = USTACKTOP - 4 * BY2PG * newthread;
	p->pcb_tf.pc = start_rountine;
	p->pcb_tf.regs[29] -= 4;
	p->pcb_tf.regs[4] = arg;
	p->pcb_tf.regs[31] = exit_thread;
	syscall_set_thread_status(p->pthread_id,ENV_RUNNABLE);
	*thread = p->pthread_id;
	if (attr) {
		if (attr -> detachstate == PTHREAD_CREATE_DETACHED || attr -> detachstate == PTHREAD_CREATE_JOINABLE) {
			p -> attr.detachstate = attr -> detachstate;
		} else return -E_INVAL_ATTR;
		if (attr -> schedpolocy == SCHED_OTHER || attr -> schedpolocy == SCHED_FIFO || attr -> schedpolocy == SCHED_RR) {
			p -> attr.schedpolocy = attr -> schedpolocy;
		} else return -E_INVAL_ATTR;
		if (attr -> schedpriority >= PTHREAD_MIN_PRIORITY && attr -> schedpriority <= PTHREAD_MAX_PRIORITY) {
			p -> attr.schedpriority = attr -> schedpriority;
		} else return -E_INVAL_ATTR;
		if (p -> attr.schedpolocy != SCHED_OTHER) {
			if (attr -> schedrtpriority >= PTHREAD_MIN_RTPRIORITY && attr -> schedrtpriority <= PTHREAD_MAX_RTPRIORITY) {
				p -> attr.schedrtpriority = attr -> schedrtpriority;
			} else return -E_INVAL_ATTR;
		}
	}
	return 0;
}

void pthread_exit(void *value_ptr) {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0xf];
	p->pcb_exit_ptr = value_ptr;
	// exit();	
    syscall_thread_destroy(0);
}

int pthread_setcancelstate(int state, int *oldvalue) {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0xf];
    if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE) {
        return -1;
    }
	if (p->pthread_id != threadid) {
		return -1;
	}
	if (oldvalue != 0) {
		*oldvalue = p->pcb_cancelState;
	}
	p->pcb_cancelState = state;
	return 0;
}

int pthread_setcanceltype(int type, int *oldvalue) {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0xf];
	if ((type != PTHREAD_CANCEL_DEFERRED) && (type != PTHREAD_CANCEL_ASYNCHRONOUS)) {
		return -1;
	}
	if (p->pthread_id != threadid) {
		return -1;
	}
	if (oldvalue != 0) {
		*oldvalue = p->pcb_cancelType;
	}
	p->pcb_cancelType = type;
	return 0;
}

void pthread_testcancel() {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0xf];
	if (p->pthread_id != threadid) {
		user_panic("panic at pthread_testcancel!\n");
	}
	if ((p->pcb_canceled)&&(p->pcb_cancelState == PTHREAD_CANCEL_ENABLE)&&(p->pcb_cancelType == PTHREAD_CANCEL_DEFERRED)) {
		//exit();
        p -> pcb_exit_value = -PTHREAD_CANCELED_EXIT;
		syscall_thread_destroy(0);
	}
}

int pthread_cancel(pthread_t thread) {
	struct Pcb *p = &env->env_pthreads[thread&0xf];
	if ((p->pthread_id != thread)|(p->pcb_status == PTHREAD_FREE)) {
		return -E_PTHREAD_NOTFOUND;
	}
	if (p->pcb_cancelState == PTHREAD_CANCEL_DISABLE) {
		return -E_PTHREAD_CANNOTCANCEL;
	}
	p -> pcb_exit_value = -PTHREAD_CANCELED_EXIT;
	if (p->pcb_cancelType == PTHREAD_CANCEL_ASYNCHRONOUS) {
		syscall_thread_destroy(thread);
	} else {
		p->pcb_canceled = 1;
	}
	return 0;
}

int pthread_detach(pthread_t thread) {
	struct Pcb * p = &env -> env_pthreads[thread & 0xf];
	int r;
	int i;
	if (p -> pthread_id != thread) {
		return -E_PTHREAD_NOTFOUND;
	}
	if (p -> pcb_status == PTHREAD_FREE) {
		u_int sp = USTACKTOP - PDMAP * (thread & 0xf);
		for (i = 1; i <= 1024; i++) {
			if ((r = syscall_mem_unmap(0, sp - i * BY2PG)) < 0) {
				return r;
			}
		}
		user_bzero(p, sizeof(struct Pcb));
	} else {
		// p -> pcb_detach = 1;
		p -> attr.detachstate = PTHREAD_CREATE_DETACHED;
	}
	return 0;
}

int pthread_join(pthread_t thread, void ** value_ptr) {
	int r = syscall_thread_join(thread, value_ptr);
	return r;
}

