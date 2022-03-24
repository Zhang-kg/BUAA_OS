/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
#define BY2PG 4096
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
                ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
                ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
                ehdr->e_ident[EI_MAG3] == ELFMAG3) {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

        int Nr;

        Elf32_Phdr *phdr = NULL;
		
        u_char *ptr_ph_table = NULL;
        Elf32_Half ph_entry_count;
        Elf32_Half ph_entry_size;


        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary)) {
                printf("not a standard elf format\n");
                return 0;
        }

        // get section table addr, section header number and section header size.
		ptr_ph_table 	= (binary + ehdr->e_phoff);
		ph_entry_count 	= ehdr->e_phnum;
		ph_entry_size 	= ehdr->e_phentsize;
		Elf32_Phdr* table[10] = {0};
		for (Nr = 0; Nr < ph_entry_count; Nr++) {
			table[Nr] = (Elf32_Phdr*)(ptr_ph_table + Nr * ph_entry_size);
		}	
		for (int i = 0; i < ph_entry_count; i++) {
			for (int j = i; j < ph_entry_count; j++) {
				if (table[i]->p_vaddr > table[j]->p_vaddr) {
					Elf32_Phdr * middle = table[i];
					table[i] = table[j];
					table[j] = middle;
				}
			}
		}
		//for (int i = 0; i < ph_entry_count; i++) {


		
		for (Nr = 0; Nr < ph_entry_count; Nr++) {
		//	phdr = (Elf32_Phdr*)(ptr_ph_table + Nr * ph_entry_size);
			phdr = table[Nr];
			if (Nr == ph_entry_count - 1) {
				printf("%d:0x%x,0x%x\n", Nr, phdr->p_filesz, phdr->p_memsz);
				continue;
			}
			int l1 = phdr->p_vaddr;
			int r1 = l1 + phdr->p_memsz;
		//	Elf32_Phdr* nexphdr = (Elf32_Phdr*)(ptr_ph_table + (Nr + 1) * ph_entry_size);
			Elf32_Phdr* nexphdr = table[Nr + 1];
			int l2 = nexphdr->p_vaddr;
			int r2 = l2 + nexphdr->p_memsz;
			if (ROUNDDOWN(r1, BY2PG) == ROUNDDOWN(l2, BY2PG)) {
				if (l2 >= r1) {
					printf("Overlay at page va : 0x%x\n", ROUNDDOWN(r1, BY2PG));
				}
				else if (l2 < r1) {
					printf("Conflict at page va : 0x%x\n", ROUNDDOWN(l2, BY2PG));
				}
			}
			else {
				printf("%d:0x%x,0x%x\n", Nr, phdr->p_filesz, phdr->p_memsz);
			}
		}
		//for (Nr = 0; Nr < sh_entry_count; Nr++)  {
		//	shdr = (Elf32_Shdr*)(ptr_sh_table + Nr * sh_entry_size);
		//	printf("%d:0x%x\n", Nr, shdr->sh_addr);
		//}
		//for (Nr = 2; Nr <= 3; Nr ++) {
		//	shdr = (Elf32_Shdr*)(ptr_sh_table + Nr * sh_entry_size);
    	//	printf("Read : %d:0x%x,0x%x\n", Nr, shdr->sh_offset, shdr->sh_addr);
		//}    
	

		// for each section header, output section number and section addr. 
        // hint: section number starts at 0.


        return 0;
}

