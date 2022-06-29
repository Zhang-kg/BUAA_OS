#include "lib.h"
#include <mmu.h>
#include <env.h>

void
exit(void)
{
	//close_all();
	syscall_env_destroy(0);
}
void exit_thread(void) {
	struct Pcb *p = &env->env_pthreads[syscall_get_threadid()&0x7];
	p->pcb_exit_value = 0;
	syscall_thread_destroy(0);
}

struct Env *env;
struct Pcb * pcb;
void
libmain(int argc, char **argv)
{
	// set env to point at our env structure in envs[].
	env = 0;	// Your code here.
	pcb = 0;
	//writef("xxxxxxxxx %x  %x  xxxxxxxxx\n",argc,(int)argv);
	int envid;
	envid = syscall_getenvid();
	envid = ENVX(envid);
	env = &envs[envid];
	int pcbid;
	pcbid = syscall_get_threadid();
	pcbid = pcbid & 0x7;
	pcb = &env->env_pthreads[pcbid];
	// call user main routine
	umain(argc, argv);
	// exit gracefully
	exit();
	//syscall_thread_destroy(0);
	//syscall_env_destroy(0);
}

