#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

LEAF(exit_thread)
    move a0, v0
    j exit_thread_final
    nop
END(exit_thread)
