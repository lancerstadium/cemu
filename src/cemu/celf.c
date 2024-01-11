/**
 * @file celf.c
 * @author your name (you@domain.com)
 * @brief ELF解析实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */



// ==================================================================== //
//                             Include
// ==================================================================== //

#include "celf.h"
#include "dram.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


// ==================================================================== //
//                            Func API: ELF
// ==================================================================== //


char* elf_arch(int arch) {
    switch(arch){
        case(0x00):
        return "NA";
        case(0x02):
        return "SPARC";
        case(0x03):
        return "x86";
        case(0x08):
        return "MIPS";
        case (0x14):
        return "PowerPC";
        case(0x16):
        return "S390";
        case(0x28):
        return	"ARM";
        case (0x2A):
        return "SuperH";
        case (0x32):
        return	"IA-64";
        case  (0x3E):
        return "x86-64";
        case (0xB7):
        return "AArch64";
        case(0xF3):
        return "RISC-V";
    }
}

char* read_section(int fd, Elf64_Shdr sh, int buff) {
    log_assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset, "lseek fail");
    log_assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size, "read fail");
	return buff;
}


u16 find_segment(void *mmaped_elf, int fsize) {
	Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) mmaped_elf;
	Elf64_Shdr* sh_table = (Elf64_Shdr*) mmaped_elf;

	int segments = elf_hdr->e_shnum;
	printf("Str Table Offest : %p\n", elf_hdr->e_shstrndx);
	char* sh_str = read_section(mmaped_elf, sh_table[elf_hdr->e_shstrndx], fsize);

	// auto sh_str = sh_table[elf_hdr->e_shstrndx]; // ELF String table

	Elf64_Phdr* phdr = (Elf64_Phdr*) (mmaped_elf + elf_hdr->e_phoff);
	printf("Segments    : %d\n", segments);

	for(int i = 0; i < segments; i++){
		if(strcmp(".text", (sh_str + sh_table[i].sh_name))) {
			printf(".text found :: %s", (sh_str + sh_table[i].sh_name));
		}
	};	
}

int get_file_size(int fd){
    struct stat st;
    fstat(fd, &st);
    return st.st_size;
}

void* map_elf(char* file_name, void* addr, int* file_len) {
    int size;
    int fd;
    if((fd = open(file_name, O_RDONLY, 0)) < 0){
        log_error("File not found\n");
        exit(1);
    }
    size = get_file_size(fd);
    printf("File Size    : %d\n", size);

    
    void* data = (void*)mmap(addr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if(data == MAP_FAILED) {
        log_error("Failed to mmap file\n");
        exit(1);
    }
    *file_len = size;
    printf("Mapped Addr  : %p\n", data);

    // 关闭文件
    if (close(fd) == -1) {
        log_error("close");
        exit(1);
    }
    return data;
}