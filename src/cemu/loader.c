/**
 * @file loader.c
 * @author lancer (lancerstadium@163.com)
 * @brief 文件加载实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */

// ==================================================================== //
//                               Include
// ==================================================================== //

#include "loader.h"
#include "celf.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>


// ==================================================================== //
//                        Private Func: Loader
// ==================================================================== //


static void copy_to_addr(char* filename, void* addr) {
    FILE *file;
    u8 *buffer;
    unsigned long filelen;
    // 1. 打开文件
    file = fopen(filename, "rb");
    if (!file) {
        log_error("Unable to open file %s", filename);
        exit(1);
    }
    // 2. 获取文件大小
    fseek(file, 0, SEEK_END);
    filelen=ftell(file);
    fseek(file, 0, SEEK_SET);
    // 3. 申请内存
    buffer=(u8 *)malloc(filelen+1);
    if (!buffer) {
        log_error("Memory error!");
        fclose(file);
    }
    // 打印
    printf("File Name    : %s\n", filename);
    printf("DRAM Memory  : %p\n", addr);

    // 4. 将文件内容读取到缓冲区
    fread(buffer, filelen, 1, file);
    fclose(file);
    // 5. 十六进制打印内容
    // for (int i=0; i<filelen; i+=2) {
    //     if (i%16==0) printf("\n0x%.8x: ", i + DRAM_BASE);
    //     printf("%02x%02x ", *(buffer+i), *(buffer+i+1));
    // }
    // printf("\n");
    // 6. 将可执行文件加载到DRAM
    memcpy(addr, buffer, filelen*sizeof(u8));
    free(buffer);
}


static void mmap_to_addr(char* filename, void* addr) {
    int fd;
    struct stat fileStat;
    void *fileData;

    // Open the file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        log_error("Unable to open file %s\n", filename);
        exit(1);
    }

    // Get the file size
    if (fstat(fd, &fileStat) == -1) {
        log_error("Unable to get file size");
        close(fd);
        exit(1);
    }

    printf("File Name    : %s\n", filename);
    printf("DRAM Memory  : %p\n", addr);

    // Map the file to memory
    fileData = mmap(addr, fileStat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, fd, 0);
    if (fileData == MAP_FAILED) {
        log_error("Unable to map file to memory");
        close(fd);
        exit(1);
    }

    // Close the file
    close(fd);
    
    printf("Option Addr  : %p\n", mmu_GPA_to_HVA(addr, DRAM_BASE));
    printf("Mapped Addr  : %p\n", fileData);
}




// ==================================================================== //
//                           Func API: loader
// ==================================================================== //

void load_file(CPU* cpu, char* filename) {
    copy_to_addr(filename, cpu->bus.dram.mem_addr);
}


void load_elf(CPU* cpu, char* filename) {
    int fsize;
    setbuf(stdout, NULL);
    void *mmaped_elf = cpu->bus.dram.mem_addr;
    // copy_to_addr(filename, mmaped_elf);
    // mmap_to_addr(filename, mmaped_elf);

    FILE *file;
    u8 *buffer;
    unsigned long filelen;
    // 1. 打开文件
    file = fopen(filename, "rb");
    if (!file) {
        log_error("Unable to open file %s", filename);
        exit(1);
    }
    // 2. 获取文件大小
    fseek(file, 0, SEEK_END);
    filelen=ftell(file);
    fseek(file, 0, SEEK_SET);
    // 3. 申请内存
    buffer=(u8 *)malloc(filelen+1);
    if (!buffer) {
        log_error("Memory error!");
        fclose(file);
    }
    // 4. 将文件内容读取到缓冲区
    fread(buffer, filelen, 1, file);
    fclose(file);
    // 6. 将可执行文件加载到DRAM
    dram_alloc_data(&cpu->bus.dram, filelen * sizeof(u8), buffer);
    // memcpy(mmaped_elf, buffer, filelen * sizeof(u8));


    Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)mmaped_elf;
    Elf64_Phdr *elf_pdr = (Elf64_Phdr *)mmaped_elf;
    uint16_t oldEntryPoint = elf_pdr->p_paddr;
    cpu->pc += elf_hdr->e_entry;

    printf("File Name    : %s\n", filename);
    printf("File Size    : %d\n", filelen);
    printf("File Ident   : %s\n", elf_hdr->e_ident);
    printf("Architecture : %s\n", elf_arch(elf_hdr->e_machine));
    printf("Entry Point  : 0x%.8lx\n", elf_hdr->e_entry);
    printf("DRAM Memory  : %p\n", mmaped_elf);
    printf("PC           : 0x%.8lx\n", cpu->pc);
    // printf("TARGET E_IDENT :: %x\n", elf_hdr->e_ident[EI_CLASS]);
    
    
    // uint16_t textSegment = find_segment(mmaped_elf, &fsize);

    // elf_pdr->p_paddr = 0x540;
}