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
    
    /*  hint:
     *  1. if (count==0), insert `e` into `env_sched_list[1-point]`
     *     using LIST_REMOVE and LIST_INSERT_TAIL.
     *  2. if (env_sched_list[point] is empty), point = 1 - point;
     *     then search through `env_sched_list[point]` for a runnable env `e`, 
     *     and set count = e->env_pri
     *  3. count--
     *  4. env_run()
     *
     *  functions or macros below may be used (not all):
     *  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
     */
//	struct Env * e;
//	printf("point = %d, e = %x\n",point, e);
//	e = LIST_FIRST(env_sched_list + point);
//	if (count == 0) {
		//printf("count = 0\n");
		//printf("bef %x\n", LIST_FIRST(&env_sched_list[point]));
		//printf("e = %x\n", e);
//		LIST_REMOVE(e, env_sched_link);
		//printf("aft %x\n", LIST_FIRST(&env_sched_list[point]));
//		LIST_INSERT_TAIL(&env_sched_list[1-point], e, env_sched_link);
//	}
//	if (LIST_EMPTY(&env_sched_list[point])) {
//		printf("list changed\n");
//		point = 1 - point;
//	}
//	struct Env * ee = NULL;
//	LIST_FOREACH(e, &env_sched_list[point], env_sched_link) {
//		if (e == NULL) printf("dfadsfa\n");
//		if (e -> env_status == ENV_RUNNABLE && ee == NULL) {
//			ee = e;
//			count = count == 0? e -> env_pri: count;
//		}
//	}
//	count--;
//	if (ee != NULL)
//		env_run(ee);
	//LIST_REMOVE(ee, env_link);
	static struct Env * e = NULL;
	if (count == 0 || e == NULL || e -> env_status != ENV_RUNNABLE) {
		if (e != NULL) {
			LIST_REMOVE(e, env_sched_link);
			if (e -> env_status != ENV_FREE) LIST_INSERT_TAIL(&env_sched_list[1-point], e, env_sched_link);
		}
		if (LIST_EMPTY(&env_sched_list[point])) {
			point = 1 - point;
		}
		LIST_FOREACH(e, &env_sched_list[point], env_sched_link) {
			if (e -> env_status == ENV_RUNNABLE) {
				count = e -> env_pri;
				break;
			}
		}
	}
	count--;
	env_run(e);

//env_run(LIST_FIRST(env_sched_list));
}
