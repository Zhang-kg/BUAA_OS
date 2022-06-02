/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	u_int dev_addr = 0x13000000;
	u_char status = 0;
	u_char read_value = 0;
	while (offset_begin + offset < offset_end) {
		// Your code here
		// error occurred, then panic.
		u_int now_offset = offset_begin + offset;
		if (syscall_write_dev((u_int)&diskno, dev_addr + 0x10, 4) < 0) {
			user_panic("ide read: diskno write err\n");
		}
		if (syscall_write_dev(&now_offset, dev_addr, 4) < 0) {
			user_panic("ide read: offset write err\n");
		}
		if (syscall_write_dev(&read_value, dev_addr + 0x20, 1) < 0) {
			user_panic("ide read: write value write err\n");
		}
		status = 0;
		if (syscall_read_dev((u_int)&status, dev_addr + 0x30, 1) < 0) {
			user_panic("ide read: read status err\n");
		}
		if (status == 0) {
			user_panic("ide read: read status err\n");
		}
		if (syscall_read_dev((u_int)(dst + offset), dev_addr + 0x4000, 0x200) < 0) {
			user_panic("ide read: get data err\n");
		}
		offset += 0x200;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	int dev_addr = 0x13000000;
	u_char status = 0;
	u_char write_value = 1;
	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	while (offset_begin + offset < offset_end) {
		// copy data from source array to disk buffer.
		int now_offset = offset_begin + offset;
		if (syscall_write_dev((u_int)&diskno, dev_addr + 0x10, 4) < 0) {
			user_panic("ide write: diskno write err\n");
		}
		if (syscall_write_dev((u_int)&now_offset, dev_addr, 4) < 0) {
			user_panic("ide write: offset write err\n");
		}
		if (syscall_write_dev((u_int)(src + offset), dev_addr + 0x4000, 0x200) < 0) {
			user_panic("ide write : prepare data err\n");
		}
		if (syscall_write_dev((u_int)&write_value, dev_addr + 0x20, 1) < 0) {
			user_panic("ide write: write process err\n");
		}
		status = 0;
		if (syscall_read_dev(&status, dev_addr + 0x30, 1) < 0) {
			user_panic("ide write: get status err\n");
		}
		if (status == 0) {
			user_panic("ide write: status is zero\n");
		}
		offset += 0x200;
		// if error occur, then panic.
	}
}

int time_read() {
	int addr = 0x15000000;
	int offset = 0x0010;
	int time = 1;
	syscall_write_dev(&time, addr, 4);
	syscall_read_dev(&time, addr + offset, 4);
	return time;
}

void raid0_write(u_int secno, void * src, u_int nsecs) {
	int finalNo = secno + nsecs;
	for (secno; secno < finalNo; secno++) {
		if (secno % 2 == 1) {
			ide_write(2, (secno - 1) / 2, src, 1);
		} else {
			ide_write(1, secno / 2, src, 1);
		}
		src += 0x200;
	}
}

void raid0_read(u_int secno, void * dst, u_int nsecs) {
	int finalNo = secno + nsecs;
	for (secno; secno < finalNo; secno++) {
		if (secno % 2 == 1) {
			ide_read(2, (secno-1)/2, dst, 1);
		} else {
			ide_read(1, secno/2, dst, 1);
		}
		dst += 0x200;
	}
}
