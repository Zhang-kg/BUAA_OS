#include <env.h>
#include <pmap.h>
#include <printf.h>
void sched_yield(void) {
	static int count = 0;
	int point = 0;
	static struct Pcb * p = NULL;
	struct Pcb * loopp;
	if (count <= 0 || p == NULL || p -> pcb_status != PTHREAD_RUNNABLE) {
		if (p != NULL) {
			LIST_REMOVE(p, pcb_sched_link);
			if (p -> pcb_status != PTHREAD_FREE) {
				LIST_INSERT_TAIL(pcb_sched_list, p, pcb_sched_link);
			}
			//LIST_FOREACH(loopp, pcb_sched_list, pcb_sched_link) {
			//	printf("%b", loopp -> pthread_id);
			//}
			//printf("\n");
		}
		int maxweight = 0;
		struct Pcb * maxp = p;
		if (p != NULL) {
			maxweight = PTHREAD_SCHED_WEIGHT(p) - 1;
			if (p -> attr.schedpolocy == SCHED_RR) maxweight -= 1;
			if (p -> attr.schedpolocy == SCHED_FIFO) maxweight += 1;
		}
		LIST_FOREACH(loopp, pcb_sched_list, pcb_sched_link) {
			int loopweight = PTHREAD_SCHED_WEIGHT(loopp);
			if (loopp != p && loopweight > maxweight && loopp -> pcb_status == ENV_RUNNABLE) {
				maxweight = loopweight;
				maxp = loopp;
			}
		}
		count = (maxp -> attr.schedpolocy == SCHED_OTHER)? maxp -> attr.schedpriority : maxp -> attr.schedrtpriority;
		LIST_REMOVE(maxp, pcb_sched_link);
		LIST_INSERT_HEAD(pcb_sched_list, maxp, pcb_sched_link);
		//LIST_FOREACH(loopp, pcb_sched_list, pcb_sched_link) {
		//	printf("%b", loopp -> pthread_id);
		//}
		//printf("\n");
	}
	if (LIST_FIRST(pcb_sched_list) != NULL) {
		count--;
		p = LIST_FIRST(pcb_sched_list);
		env_run(LIST_FIRST(pcb_sched_list));
	}
}
