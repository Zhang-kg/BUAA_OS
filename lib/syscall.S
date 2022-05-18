#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <stackframe.h>
#include <unistd.h>

/*** exercise 4.2 ***/
NESTED(handle_sys,TF_SIZE, sp)
    SAVE_ALL                            // Macro used to save trapframe
    CLI                                 // Clean Interrupt Mask
    nop
    .set at                             // Resume use of $at

    // TODO: Fetch EPC from Trapframe, calculate a proper value and store it back to trapframe.
	lw t0, TF_EPC(sp)
	addi t0, t0, 4
	sw t0, TF_EPC(sp)
    // TODO: Copy the syscall number into $a0.
	lw a0, TF_REG4(sp)
	
    addiu   a0, a0, -__SYSCALL_BASE     // a0 <- relative syscall number
    sll     t0, a0, 2                   // t0 <- relative syscall number times 4
    la      t1, sys_call_table          // t1 <- syscall table base
    addu    t1, t1, t0                  // t1 <- table entry of specific syscall
    lw      t2, 0(t1)                   // t2 <- function entry of specific syscall

	lw t0, TF_REG29(sp)
	lw s0, TF_REG5(sp)
	addiu s2, sp, 0
	LOOP_BEGIN:
		ble s0, 3, LOOP_END
		
		addiu t1, s0, 1
		sll t1, t1, 2
		addu t1, t1, t0
		lw s1, 0(t1)
		addiu sp, sp, -4
		sw s1, 0(sp)
		
		addiu s0, s0, -1
		j LOOP_BEGIN
		nop
		
	LOOP_END:
		addiu sp, sp, -16
		lw a0, TF_REG4(s2)
		lw a1, TF_REG6(s2)
		lw a2, TF_REG7(s2)
		lw a3, 16(t0)


    jalr    t2                          // Invoke sys_* function
    nop

    // TODO: Resume current kernel stack
	move sp, s2

    sw      v0, TF_REG2(sp)             // Store return value of function sys_* (in $v0) into trapframe

    j       ret_from_exception          // Return from exeception
    nop
END(handle_sys)

sys_call_table:                         // Syscall Table
    .align 2
    .word sys_putchar
    .word sys_getenvid
    .word sys_yield
    .word sys_env_destroy
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_env_alloc
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_cgetc
	.word sys_sum
