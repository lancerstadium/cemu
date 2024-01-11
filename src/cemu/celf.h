/**
 * @file celf.h
 * @author your name (you@domain.com)
 * @brief ELF解析头文件
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef C_ELF_H
#define C_ELF_H

// ==================================================================== //
//                             Include
// ==================================================================== //

#include <stdint.h>
#include <stdbool.h>
#include <elf.h>
#include "utils.h"


// ==================================================================== //
//                            Declare: ELF
// ==================================================================== //


char* elf_arch(int arch);
char* read_section(int fd, Elf64_Shdr sh, int buff);
u16 find_segment(void *mmaped_elf, int fsize);
int get_file_size(int fd);
void* map_elf(char* file_name, void* addr, int* file_len);


#endif /* ELF_H */