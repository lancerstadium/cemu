/**
 * @file dram.c
 * @author lancer (lancerstadium@163.com)
 * @brief DRAM功能实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */


// ==================================================================== //
//                             Include
// ==================================================================== //


#include "dram.h"
#include "log.h"


// ==================================================================== //
//                         Private Func: DRAM
// ==================================================================== //


static inline u64 dram_load_8(DRAM* dram, u64 addr) {
    return (u64) dram->mem_addr[addr-DRAM_BASE];
}

/**
 * @note 小端法是将最小端（权值最低位）最先存储。
 * 当读取数据时，我们先读取最低的地址中存放的值通过返回值输送给总线，
 * 随后左移 8 位（1 字节）并与 0xff（8 个 1）进行与操作来得到最低字节的数据，
 * 并清空高字节以便用于或操作。
 */
static inline u64 dram_load_16(DRAM* dram, u64 addr) {
    return (u64) dram->mem_addr[addr-DRAM_BASE]
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8;
}

static inline u64 dram_load_32(DRAM* dram, u64 addr) {
    return (u64) dram->mem_addr[addr-DRAM_BASE]
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 16
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 24;
}

static inline u64 dram_load_64(DRAM* dram, u64 addr) {
    return (u64) dram->mem_addr[addr-DRAM_BASE]
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 16
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 24
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 32
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 40
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 48
        | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 56;
}

static inline void dram_store_8(DRAM* dram, u64 addr, u64 value) {
    dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
}
static inline void dram_store_16(DRAM* dram, u64 addr, u64 value) {
    dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
}
static inline void dram_store_32(DRAM* dram, u64 addr, u64 value) {
    dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 2]   = (u8) ((value >> 16) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 3]   = (u8) ((value >> 24) & 0xff);
}
static inline void dram_store_64(DRAM* dram, u64 addr, u64 value) {
    dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 2]   = (u8) ((value >> 16) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 3]   = (u8) ((value >> 24) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 4]   = (u8) ((value >> 32) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 5]   = (u8) ((value >> 40) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 6]   = (u8) ((value >> 48) & 0xff);
    dram->mem_addr[addr-DRAM_BASE + 7]   = (u8) ((value >> 56) & 0xff);
}


// ==================================================================== //
//                            Func API: DRAM
// ==================================================================== //


void dram_init(DRAM* dram) {
    dram->mem_addr = (uint8_t*)malloc(DRAM_SIZE);  // 分配DRAM的内存空间
    dram->alloc_size = 0;
    dram->alloc_addr = dram->mem_addr;  // 初始时，待分配地址指向DRAM的起始位置
    log_info("DRAM mem addr: %p", dram->mem_addr);
}


void dram_alloc_data(DRAM* dram, size_t size, void* data) {
    if (dram->alloc_addr + size > dram->mem_addr + DRAM_SIZE) {
        // 如果待分配的地址超出了DRAM的范围
        // 处理错误，这里简单地打印错误信息
        log_error("Out of memory range");
        return;
    }

    // 将数据加载到DRAM
    memcpy(dram->alloc_addr, data, size);
    // 更新已分配大小和待分配地址的指针
    dram->alloc_size += size;
    dram->alloc_addr += size;
    log_info("DRAM alloced: %d now at (0x%.8x)", size, DRAM_BASE + dram->alloc_size);
}

void dram_write_data(DRAM* dram, size_t offset, size_t size, u64 value) {
    if (offset + size > dram->alloc_size) {
        // 如果写入的偏移量和大小超出了已分配的范围
        // 处理错误，这里简单地打印错误信息
        log_error("Writing out of allocated range");
        return;
    }
    // 写入数据到DRAM
    // memcpy(dram->mem_addr + offset, data_addr, size);
    u64 addr = DRAM_BASE + offset;
    switch (size) {
        case  8:    dram_store_8(dram, addr, value);  break;
        case 16:    dram_store_16(dram, addr, value); break;
        case 32:    dram_store_32(dram, addr, value); break;
        case 64:    dram_store_64(dram, addr, value); break;
        default:;
    }
    log_info("DRAM write: %d to (0x%.8x)", size, addr);
}


u64 dram_load_data(DRAM* dram, size_t offset, size_t size) {
    if (offset + size > dram->alloc_size) {
        // 如果取出的偏移量和大小超出了已分配的范围
        // 处理错误，这里简单地打印错误信息
        log_error("Loading out of allocated range");
        return;
    }
    // 从DRAM中取出数据
    // memcpy(data_addr, dram->mem_addr + offset, size);
    u64 addr = DRAM_BASE + offset;
    u64 data_addr = 1;
    switch (size) {
        case  8:    data_addr = dram_load_8(dram, addr); break;
        case 16:    data_addr = dram_load_16(dram, addr);break;
        case 32:    data_addr = dram_load_32(dram, addr);break;
        case 64:    data_addr = dram_load_64(dram, addr);break;
        default:;
    }
    log_info("DRAM load: %d from (0x%.8x)", size, addr);
    return data_addr;
}


void dram_release_data(DRAM* dram, size_t size) {
    if (size > dram->alloc_size) {
        // 如果删除的大小超出了已分配的范围
        // 处理错误，这里简单地打印错误信息
        log_error("Releasing more than allocated size");
        return;
    }

    // 更新已分配大小
    dram->alloc_size -= size;
    dram->alloc_addr -= size;
}


void dram_free(DRAM* dram) {
    free(dram->mem_addr);
    size_t total_size = dram->alloc_size;
    dram->alloc_size = 0;
    dram->alloc_addr = NULL;
    log_info("DRAM free %d", total_size);
}











// ==================================================================== //
//                            Old API: DRAM
// ==================================================================== //


// static inline u64 dram_load_8(DRAM* dram, u64 addr) {
//     return (u64) dram->mem_addr[addr-DRAM_BASE];
// }

// /**
//  * @note 小端法是将最小端（权值最低位）最先存储。
//  * 当读取数据时，我们先读取最低的地址中存放的值通过返回值输送给总线，
//  * 随后左移 8 位（1 字节）并与 0xff（8 个 1）进行与操作来得到最低字节的数据，
//  * 并清空高字节以便用于或操作。
//  */
// static inline u64 dram_load_16(DRAM* dram, u64 addr) {
//     return (u64) dram->mem_addr[addr-DRAM_BASE]
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8;
// }

// static inline u64 dram_load_32(DRAM* dram, u64 addr) {
//     return (u64) dram->mem_addr[addr-DRAM_BASE]
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 16
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 24;
// }

// static inline u64 dram_load_64(DRAM* dram, u64 addr) {
//     return (u64) dram->mem_addr[addr-DRAM_BASE]
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 8
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 16
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 24
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 32
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 40
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 48
//         | (u64) dram->mem_addr[addr-DRAM_BASE + 1] << 56;
// }

// static inline void dram_store_8(DRAM* dram, u64 addr, u64 value) {
//     dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
// }
// static inline void dram_store_16(DRAM* dram, u64 addr, u64 value) {
//     dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
// }
// static inline void dram_store_32(DRAM* dram, u64 addr, u64 value) {
//     dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 2]   = (u8) ((value >> 16) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 3]   = (u8) ((value >> 24) & 0xff);
// }
// static inline void dram_store_64(DRAM* dram, u64 addr, u64 value) {
//     dram->mem_addr[addr-DRAM_BASE]       = (u8) (value & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 1]   = (u8) ((value >> 8) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 2]   = (u8) ((value >> 16) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 3]   = (u8) ((value >> 24) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 4]   = (u8) ((value >> 32) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 5]   = (u8) ((value >> 40) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 6]   = (u8) ((value >> 48) & 0xff);
//     dram->mem_addr[addr-DRAM_BASE + 7]   = (u8) ((value >> 56) & 0xff);
// }

// /**
//  * @note 为了获取`addr`参数给定的地址上的数据，
//  * 我们需要从`addr`中减去`DRAM_BASE`。
//  * 数据的实际起始地址为`mem_addr[addr-DRAM_BASE]`
//  */
// u64 dram_load(DRAM* dram, u64 addr, u64 size) {
//     switch (size) {
//         case  8:    return dram_load_8(dram, addr);
//         case 16:    return dram_load_16(dram, addr);
//         case 32:    return dram_load_32(dram, addr);
//         case 64:    return dram_load_64(dram, addr);
//         default:;
//     }
//     return 1;
// }

// void dram_store(DRAM* dram, u64 addr, u64 size, u64 value) {
//     switch (size) {
//         case  8:    dram_store_8(dram, addr, value); break;
//         case 16:    dram_store_16(dram, addr, value); break;
//         case 32:    dram_store_32(dram, addr, value); break;
//         case 64:    dram_store_64(dram, addr, value); break;
//         default:;
//     }
// }

