#include "lib.h"
#include <error.h>
#include <mmu.h>

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
	p->pcb_tf.regs[31] = exit;
	syscall_set_thread_status(p->pthread_id,ENV_RUNNABLE);
	*thread = p->pthread_id;
	return 0;
}

void pthread_exit(void *value_ptr) {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0x7];
	p->pcb_exit_ptr = value_ptr;
	if (value_ptr) writef("value is %d\n", *(int *)(value_ptr));
	// exit();	
    syscall_thread_destroy(0);
}

int pthread_setcancelstate(int state, int *oldvalue) {
	u_int threadid = syscall_get_threadid();
	struct Pcb *p = &env->env_pthreads[threadid&0x7];
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
	struct Pcb *p = &env->env_pthreads[threadid&0x7];
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
	struct Pcb *p = &env->env_pthreads[threadid&0x7];
	if (p->pthread_id != threadid) {
		user_panic("panic at pthread_testcancel!\n");
	}
	if ((p->pcb_canceled)&&(p->pcb_cancelState == PTHREAD_CANCEL_ENABLE)&&(p->pcb_cancelType == PTHREAD_CANCEL_DEFERRED)) {
		//exit();
        syscall_thread_destroy(0);
	}
}

int pthread_cancel(pthread_t thread) {
	struct Pcb *p = &env->env_pthreads[thread&0x7];
	if ((p->pthread_id != thread)|(p->pcb_status == PTHREAD_FREE)) {
		return -E_PTHREAD_NOTFOUND;
	}
	if (p->pcb_cancelState == PTHREAD_CANCEL_DISABLE) {
		return -E_PTHREAD_CANNOTCANCEL;
	}
	if (p->pcb_cancelType == PTHREAD_CANCEL_ASYNCHRONOUS) {
		syscall_thread_destroy(thread);
	} else {
		p->pcb_canceled = 1;
	}
	return 0;
}
