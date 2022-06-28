#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.15 ***/
void sched_yield(void)
{
    static int count = 0; // remaining time slices of current env
    static int point = 0; // current env_sched_list index
	static struct Pcb * p = NULL;
//	printf("in the sched\n");
//	printf("count = %d\n", count);
	if (count == 0 || p == NULL || p -> pcb_status != PTHREAD_RUNNABLE) {
		if (p != NULL) {
			LIST_REMOVE(p, pcb_sched_link);
			if (p -> pcb_status != PTHREAD_FREE)
				LIST_INSERT_TAIL(&pcb_sched_list[1 - point], p, pcb_sched_link);
		}
		while (1) {
			while (LIST_EMPTY(&pcb_sched_list[point])) {
				point = 1 - point;
			}
			p = LIST_FIRST(&pcb_sched_list[point]);
			if (p -> pcb_status == PTHREAD_FREE) {
				LIST_REMOVE(p, pcb_sched_link);
			} else if (p -> pcb_status == PTHREAD_NOT_RUNNABLE) {
				LIST_REMOVE(p, pcb_sched_link);
				LIST_INSERT_TAIL(&pcb_sched_list[1 - point], p, pcb_sched_link);
			} else {
				count = p -> pcb_pri;
				break;
			}
		}
	}
//	printf("final p is %b\n", p -> pthread_id);
	if (p != NULL) {
		count--;
		env_run(p);
	}
}

