#include "../drivers/gxconsole/dev_cons.h"
#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>

extern char *KERNEL_SP;
extern struct Env *curenv;

/* Overview:
 * 	This function is used to print a character on screen.
 *
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
	printcharc((char) c);
	return ;
}

/* Overview:
 * 	This function enables you to copy content of `srcaddr` to `destaddr`.
 *
 * Pre-Condition:
 * 	`destaddr` and `srcaddr` can't be NULL. Also, the `srcaddr` area
 * 	shouldn't overlap the `destaddr`, otherwise the behavior of this
 * 	function is undefined.
 *
 * Post-Condition:
 * 	the content of `destaddr` area(from `destaddr` to `destaddr`+`len`) will
 * be same as that of `srcaddr` area.
 */
void *memcpy(void *destaddr, void const *srcaddr, u_int len)
{
	char *dest = destaddr;
	char const *src = srcaddr;

	while (len-- > 0) {
		*dest++ = *src++;
	}

	return destaddr;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
u_int sys_getenvid(void)
{
	return curenv->env_id;
}

/* Overview:
 *	This function enables the current process to give up CPU.
 *
 * Post-Condition:
 * 	Deschedule current environment. This function will never return.
 */
/*** exercise 4.6 ***/
void sys_yield(void)
{
	bcopy((void*)(KERNEL_SP - sizeof(struct Trapframe)), (void*)(TIMESTACK - sizeof(struct Trapframe)), sizeof(struct Trapframe));
    sched_yield();
}

/* Overview:
 * 	This function is used to destroy the current environment.
 *
 * Pre-Condition:
 * 	The parameter `envid` must be the environment id of a
 * process, which is either a child of the caller of this function
 * or the caller itself.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 when error occurs.
 */
int sys_env_destroy(int sysno, u_int envid)
{
	/*
		printf("[%08x] exiting gracefully\n", curenv->env_id);
		env_destroy(curenv);
	*/
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0) {
		return r;
	}

	printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

/* Overview:
 * 	Set envid's pagefault handler entry point and exception stack.
 *
 * Pre-Condition:
 * 	xstacktop points one byte past exception stack.
 *
 * Post-Condition:
 * 	The envid's pagefault handler will be set to `func` and its
 * 	exception stack will be set to `xstacktop`.
 * 	Returns 0 on success, < 0 on error.
 */
/*** exercise 4.12 ***/
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{
	// Your code here.
	struct Env *env;
	int ret;

	if ((ret = envid2env(envid, &env, 0)) < 0) return ret;
    env -> env_pgfault_handler = func;
    env -> env_xstacktop = xstacktop;

	return 0;
	//	panic("sys_set_pgfault_handler not implemented");
}

/* Overview:
 * 	Allocate a page of memory and map it at 'va' with permission
 * 'perm' in the address space of 'envid'.
 *
 * 	If a page is already mapped at 'va', that page is unmapped as a
 * side-effect.
 *
 * Pre-Condition:
 * perm -- PTE_V is required,
 *         PTE_COW is not allowed(return -E_INVAL),
 *         other bits are optional.
 *
 * Post-Condition:
 * Return 0 on success, < 0 on error
 *	- va must be < UTOP
 *	- env may modify its own address space or the address space of its children
 */
/*** exercise 4.3 ***/
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
	// Your code here.
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;
	
	// Pre-Condition
    if (perm & PTE_COW) return -E_INVAL;
    if ((perm & PTE_V) == 0) return -E_INVAL;
    
    if (va >= UTOP) return -E_INVAL;
    	
    // page alloc and map
    if ((ret = page_alloc(&ppage)) < 0) return ret;
	if ((ret = envid2env(envid, &env, 1)) < 0) return ret;
    if ((ret = page_insert(env -> env_pgdir, ppage, va, perm)) < 0) return ret;
  
    // Pose-Condition
    return 0;
}

/* Overview:
 * 	Map the page of memory at 'srcva' in srcid's address space
 * at 'dstva' in dstid's address space with permission 'perm'.
 * Perm has the same restrictions as in sys_mem_alloc.
 * (Probably we should add a restriction that you can't go from
 * non-writable to writable?)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Note:
 * 	Cannot access pages above UTOP.
 */
/*** exercise 4.4 ***/
int sys_mem_map(int sysno, u_int srcid, u_int srcva, u_int dstid, u_int dstva,
				u_int perm)
{
	int ret;
	u_int round_srcva, round_dstva;
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *ppage;
	Pte *ppte;

	ppage = NULL;
	ret = 0;
	round_srcva = ROUNDDOWN(srcva, BY2PG);
	round_dstva = ROUNDDOWN(dstva, BY2PG);

    //your code here
    if (!(perm & PTE_V)) return -E_INVAL;
    if (round_srcva >= UTOP || round_dstva >= UTOP) return -E_INVAL;
    if ((ret = envid2env(srcid, &srcenv, 0)) < 0) return ret;
    if ((ret = envid2env(dstid, &dstenv, 0)) < 0) return ret;
    ppage = page_lookup(srcenv -> env_pgdir, round_srcva, &ppte);
    if (ppage == 0) return -E_INVAL;
    if ((perm & PTE_R) && !((*ppte) & PTE_R)) return -E_INVAL;
    if ((ret = page_insert(dstenv -> env_pgdir, ppage, round_dstva, perm)) < 0) return ret;
	return ret;
}

/* Overview:
 * 	Unmap the page of memory at 'va' in the address space of 'envid'
 * (if no page is mapped, the function silently succeeds)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Cannot unmap pages above UTOP.
 */
/*** exercise 4.5 ***/
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
	// Your code here.
	int ret;
	struct Env *env;
	if (va >= UTOP) return -1;
    if ((ret = envid2env(envid, &env, 0)) < 0) return ret;
	page_remove(env -> env_pgdir, va);
	return ret;
	//	panic("sys_mem_unmap not implemented");
}

/* Overview:
 * 	Allocate a new environment.
 *
 * Pre-Condition:
 * The new child is left as env_alloc created it, except that
 * status is set to ENV_NOT_RUNNABLE and the register set is copied
 * from the current environment.
 *
 * Post-Condition:
 * 	In the child, the register set is tweaked so sys_env_alloc returns 0.
 * 	Returns envid of new environment, or < 0 on error.
 */
/*** exercise 4.8 ***/
int sys_env_alloc(void)
{
	// Your code here.
	int r;
	struct Env *e;
	
	if ((r = env_alloc(&e, curenv -> env_id)) < 0) return r;
    bcopy((void *)(KERNEL_SP - sizeof(struct Trapframe)), (void*)(&(e -> env_pthreads[0].pcb_tf)), sizeof(struct Trapframe));
    e -> env_pthreads[0].pcb_tf.pc = e -> env_pthreads[0].pcb_tf.cp0_epc;
    //e -> env_id = 0;
    e -> env_pthreads[0].pcb_tf.regs[2] = 0;
    e -> env_pthreads[0].pcb_status = ENV_NOT_RUNNABLE;
    // e -> env_pthreads[0].pcb_pri = curenv -> env_pthreads[0].pcb_pri;
	e -> env_pthreads[0].attr.schedpolocy = curenv -> env_pthreads[0].attr.schedpolocy;
	e -> env_pthreads[0].attr.schedpriority = curenv -> env_pthreads[0].attr.schedpriority;
	e -> env_pthreads[0].attr.schedrtpriority = curenv -> env_pthreads[0].attr.schedrtpriority;
	return e->env_id;
	//	panic("sys_env_alloc not implemented");
}

/* Overview:
 * 	Set envid's env_status to status.
 *
 * Pre-Condition:
 * 	status should be one of `ENV_RUNNABLE`, `ENV_NOT_RUNNABLE` and
 * `ENV_FREE`. Otherwise return -E_INVAL.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if status is not a valid status for an environment.
 * 	The status of environment will be set to `status` on success.
 */
/*** exercise 4.14 ***/
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
	// Your code here.
	struct Env *env;
	int ret;
	//if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE && status != ENV_FREE) return -E_INVAL;
	if (status > 2 || status < 0) return -E_INVAL;
    if ((ret = envid2env(envid, &env, 0)) < 0) return -E_INVAL;
	env -> env_pthreads[0].pcb_status = status;
    if (env -> env_pthreads[0].pcb_status == ENV_RUNNABLE) LIST_INSERT_HEAD(pcb_sched_list, &(env -> env_pthreads[0]), pcb_sched_link);
	return 0;
	//	panic("sys_env_set_status not implemented");
}

int sys_set_thread_status(int sysno, u_int threadid, u_int status) {
	struct Pcb * p;
	int ret;
	if (status > 2 || status < 0) return -E_INVAL;
	if ((ret = pthreadid2pcb(threadid, &p)) < 0) return ret;
	if (status == PTHREAD_RUNNABLE && p -> pcb_status == PTHREAD_FREE) {
		//printf("status = %d, pthread_id = %b\n", status, threadid);
		LIST_INSERT_HEAD(pcb_sched_list, p, pcb_sched_link);
	}
	p -> pcb_status = status;
}

/* Overview:
 * 	Set envid's trap frame to tf.
 *
 * Pre-Condition:
 * 	`tf` should be valid.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if the environment cannot be manipulated.
 *
 * Note: This hasn't be used now?
 */
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{

	return 0;
}

/* Overview:
 * 	Kernel panic with message `msg`.
 *
 * Pre-Condition:
 * 	msg can't be NULL
 *
 * Post-Condition:
 * 	This function will make the whole system stop.
 */
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}

/* Overview:
 * 	This function enables caller to receive message from
 * other process. To be more specific, it will flag
 * the current process so that other process could send
 * message to it.
 *
 * Pre-Condition:
 * 	`dstva` is valid (Note: NULL is also a valid value for `dstva`).
 *
 * Post-Condition:
 * 	This syscall will set the current process's status to
 * ENV_NOT_RUNNABLE, giving up cpu.
 */
/*** exercise 4.7 ***/
void sys_ipc_recv(int sysno, u_int dstva)
{
	if (dstva >= UTOP) return;
	if (curenv -> env_ipc_recving == 1) sys_yield();
    curenv -> env_ipc_recving = 1;
	curenv -> env_ipc_waiting_pthread_no = curpcb -> pthread_id & 0xf;
    curenv -> env_ipc_dstva = dstva;
    curenv -> env_pthreads[0].pcb_status = ENV_NOT_RUNNABLE;
    sys_yield();
}

/* Overview:
 * 	Try to send 'value' to the target env 'envid'.
 *
 * 	The send fails with a return value of -E_IPC_NOT_RECV if the
 * target has not requested IPC with sys_ipc_recv.
 * 	Otherwise, the send succeeds, and the target's ipc fields are
 * updated as follows:
 *    env_ipc_recving is set to 0 to block future sends
 *    env_ipc_from is set to the sending envid
 *    env_ipc_value is set to the 'value' parameter
 * 	The target environment is marked runnable again.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Hint: the only function you need to call is envid2env.
 */
/*** exercise 4.7 ***/
int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva,
					 u_int perm)
{
	int r;
    struct Env *e;
    struct Page *p;
	struct Pcb * pthread_p;
    if (srcva >= UTOP) return -E_IPC_NOT_RECV;
	if ((r = envid2env(envid, &e, 0)) < 0) return -E_IPC_NOT_RECV;
    if (!e -> env_ipc_recving) return -E_IPC_NOT_RECV;
    e -> env_ipc_recving = 0;
    if (srcva != 0) {
        if ((p = page_lookup(curenv -> env_pgdir, srcva, NULL)) == NULL) return -E_INVAL;
        if ((r = page_insert(e -> env_pgdir, p, e -> env_ipc_dstva, perm)) < 0) return r;
    }
    e -> env_ipc_from = curenv -> env_id;
    e -> env_ipc_value = value;
	pthread_p = &e -> env_pthreads[e -> env_ipc_waiting_pthread_no];
	pthread_p -> pcb_status = PTHREAD_RUNNABLE;
    // e -> env_status = ENV_RUNNABLE;
	e -> env_ipc_perm = perm;
	return 0;
}


int sys_get_threadid(int sysno) {
	return curpcb -> pthread_id;
}
int sys_thread_destroy(int sysno, u_int pthreadid) {
	int r;
	struct Pcb * p;
	if ((r = pthreadid2pcb(pthreadid, &p)) < 0) {
		return r;
	}
	if (p -> pcb_status == PTHREAD_FREE) {
		return -E_INVAL;
	}
	if (p -> pcb_joined_thread_ptr != NULL) {
		struct Pcb * tmp = p -> pcb_joined_thread_ptr;
		if (tmp -> pcb_join_value_ptr) {
			*(tmp -> pcb_join_value_ptr) = p -> pcb_exit_ptr;
		}
		sys_set_thread_status(0, tmp -> pthread_id, ENV_RUNNABLE);
		p -> pcb_joined_thread_ptr = NULL;
	}
	printf("[0x%08x] destroyint pcb 0x%08x\n", curenv -> env_id, p -> pthread_id);
	thread_destroy(p);
	return 0;
}


int sys_thread_alloc(int sysno) {
	int r;
	struct Pcb * p;
	if (curenv) {
		r = thread_alloc(curenv, &p);
	} else r = -1;
	if (r < 0) {
		return r;
	}
	p -> attr.schedpolocy = SCHED_OTHER;
	p -> attr.schedpriority = 1;
	p -> attr.schedrtpriority = 1;
	if (curenv) {
		// p -> pcb_pri = curenv -> env_pthreads[0].pcb_pri;
		p -> attr.schedpolocy = curenv -> env_pthreads[0].attr.schedpolocy;
		p -> attr.schedpriority = curenv -> env_pthreads[0].attr.schedpriority;
		p -> attr.schedrtpriority = curenv -> env_pthreads[0].attr.schedrtpriority;
	}
	p -> pcb_status = PTHREAD_NOT_RUNNABLE;
	LIST_INSERT_HEAD(pcb_sched_list, p, pcb_sched_link);
	p -> pcb_tf.regs[2] = 0;
	p -> pcb_tf.pc = p -> pcb_tf.cp0_epc;
	return p -> pthread_id & 0xf;
}

int sys_thread_join(int sysno, u_int threadid, void ** value_ptr) {
	struct Pcb * p;
	int r;
	if ((r = pthreadid2pcb(threadid, &p)) < 0) {
		return r;
	}
	if (p -> attr.detachstate == 1 || p -> pcb_joined_thread_ptr != 0) {
		return -E_PTHREAD_JOIN_FAIL;
	}
	if (p -> pcb_status == PTHREAD_FREE) {
		if (value_ptr != 0 && p -> pcb_exit_ptr != 0) {
			*value_ptr = p -> pcb_exit_ptr;
			p -> pcb_exit_ptr = 0;
		}
		return 0;
	}
	if (curpcb -> pcb_joined_thread_ptr == p) {
		return -E_PTHREAD_JOIN_FAIL;
	}
	p -> pcb_joined_thread_ptr = curpcb;
	curpcb -> pcb_join_value_ptr = value_ptr;
	sys_set_thread_status(0, curpcb -> pthread_id, PTHREAD_NOT_RUNNABLE);
	struct Trapframe * trap = (struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe));
	trap -> regs[2] = 0;
	trap -> pc = trap -> cp0_epc;
	sys_yield();
	return -E_PTHREAD_JOIN_FAIL;
}

int sys_sem_destroy(int sysno, sem_t * sem) {
	if ((sem -> sem_envid != curenv->env_id) && (sem -> sem_shared == 0)) {
		return -E_SEM_NOTFOUND;
	}
	if (sem -> sem_status == SEM_FREE) {
		return 0;
	}
	sem -> sem_status = SEM_FREE;
	return 0;
}

int sys_sem_wait(int sysno, sem_t * sem) {
	if (sem -> sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	sem -> sem_value -= 1;
	if (sem -> sem_wait_count >= 10) {
		return -E_SEM_ERROR;
	}
	if (sem -> sem_value < 0) {
		sem -> sem_wait_list[sem -> sem_head_index] = curpcb;
		sem -> sem_head_index = (sem -> sem_head_index + 1) % 10;
		sem -> sem_wait_count += 1;
		sys_set_thread_status(0, 0, PTHREAD_NOT_RUNNABLE);
		struct Trapframe *trap = (struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe));
		trap->regs[2] = 0;
		trap->pc = trap->cp0_epc;
		sys_yield();
	} else {
		return 0;
	}
	return -E_SEM_ERROR;
}

int sys_sem_trywait(int sysno, sem_t * sem) {
	if (sem -> sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	if (sem -> sem_value > 0) {
		sem -> sem_value -= 1;
		return 0;
	}
	return -E_SEM_EAGAIN;
}

int sys_sem_post(int sysno, sem_t * sem) {
	if (sem->sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	sem -> sem_value += 1;
	if (sem -> sem_value > 0) {
		return 0;
	} else {
		struct Pcb * p;
		sem -> sem_wait_count -= 1;
		p = sem -> sem_wait_list[sem -> sem_tail_index];
		sem -> sem_wait_list[sem -> sem_tail_index] = 0;
		sem -> sem_tail_index = (sem -> sem_tail_index + 1) % 10;
		sys_set_thread_status(0, p -> pthread_id, PTHREAD_RUNNABLE);
	}
	return 0;
}

int sys_sem_getvalue(int sysno, sem_t * sem, int * valuep) {
	if (sem->sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	if (valuep != 0) {
		*valuep = sem->sem_value;
	}
	return 0;
}

