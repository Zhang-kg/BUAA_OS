#include <env.h>
#include <pmap.h>
#include <printf.h>

void sched_yield(void) {
	printf("\n");
	static int count = 0;
	static int point = 0;
	static struct Env * e = NULL;
	if (count == 0 || e == NULL || e -> env_status != ENV_RUNNABLE) {
		if (e != NULL) {
			LIST_REMOVE(e, env_sched_link);
			if (e -> env_pri & 1) {
				LIST_INSERT_TAIL(&env_sched_list[(point + 1) % 3], e, env_sched_link);
			} else {
				LIST_INSERT_TAIL(&env_sched_list[(point - 1 + 3) % 3], e, env_sched_link);
			}
		}
		while (1) {
			while (LIST_EMPTY(&env_sched_list[point])) {
				point = (point + 1) % 3;
			}
			e = LIST_FIRST(&env_sched_list[point]);
			if (e -> env_status != ENV_RUNNABLE) {
				LIST_REMOVE(e, env_sched_link);
				if (e -> env_pri & 1) {
					LIST_INSERT_TAIL(&env_sched_list[(point + 1) % 3], e, env_sched_link);
				} else {
					LIST_INSERT_TAIL(&env_sched_list[(point - 1 + 3) % 3], e, env_sched_link);
				}
			} else {
				count = e -> env_pri * (1 << point);
				break;
			}
		}
	}
	if (e != NULL) {
		count--;
		env_run(e);
	}
}
