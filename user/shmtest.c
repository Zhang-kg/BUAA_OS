#include "lib.h"

void umain() {
    writef("begin\n");
	volatile u_int *a = (volatile u_int *) 0x23333334;
    int x = make_shared((void *) a);
	writef("aft1  %x\n", x);
    *a = 233;
    if (fork() == 0) {
        u_int ch = syscall_getenvid();
        *a = ch;
        while (*a == ch)
            syscall_yield();
        writef("parent is %x\n", *a);
    } else {
        while (*a == 233)
            syscall_yield();
        writef("child is %x\n", *a);
        *a = syscall_getenvid();
    }
}
