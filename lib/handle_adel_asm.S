#include <asm/regdef.h> 
#include<asm/cp0regdef.h> 
#include<asm/asm.h> 
#include<stackframe.h>

NESTED(handle_adel, TF_SIZE, sp)
	nop
	CLI
	//SAVE_ALL
	mfc0 v1, CP0_EPC
	nop
	lw v0, 0(v1)
	li k0, 0x08000000
	and k1, k0, v0
	bnez k1, loadw
	nop
	li k0, 0x04000000
	xor v0, k0
	sw v0, 0(v1)
	j fin
	nop
loadw:
	li k0, 0x08000000
	xor v0, k0
	sw v0, 0(v1)
	j fin
	nop
fin:
	//RESTORE_ALL
	STI
	mfc0 k0, CP0_EPC
	nop
	jr k0
	rfe
END(handle_adel)
