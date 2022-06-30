#include "lib.h"

void *test(void *arg) {
	int ret = -2;
	writef("son is begin\n");
	writef("son threadid is 0x%x\n",syscall_get_threadid());
	pthread_exit(&ret);
	writef("son not exit!\n");
}

void umain() {
	writef("Begin\n");
	writef("create pthread\n");
	pthread_t thread;
	int args[3];
	args[0] = 1;
	args[1] = 2;
	args[2] = 3;
	pthread_create(&thread,NULL,test,(void *)args);
	writef("son is create!\n");
	while ((int)env->env_pthreads[thread&0x7].pcb_status != ENV_FREE) {
		writef("wait ");
	}
	writef("son exit ret is %d\n",*((int *)env->env_pthreads[thread&0x7].pcb_exit_ptr));
}
