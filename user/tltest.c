#include "lib.h"

void umain() {
//    writef(
//        "Smashing some kernel codes...\n"
//        "If your implementation is correct, you may see some TOO LOW here:\n"
//    );
//    *(int *) KERNBASE = 0;
//    writef("My mission completed!\n");
	int ans = syscall_sum(1, 2, 3, 4, 5, 6);
	writef("umain ans = %d", ans);
	for(;;);
}
