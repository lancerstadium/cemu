/**
 * @file dram.h
 * @author lancer (lancerstadium@163.com)
 * @brief DRAM头文件
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 * # DRAM 介绍
 * 
 * ## DRAM 结构
 * - DRAM（动态随机存取存储器）存放所有指令与数据。
 * 模拟器中的内存只是一个 64 位变量的数组，用于存放 64 位值。
 * 此处我们用变量`DRAM_SIZE`定义内存的大小，
 * 用变量`DRAM_BASE`定义内存的起始地址。
 * 
 * - 内存有一个大于`0x0`的起始地址，
 * 因为 RISC-V 架构有一个内存映射 I/O。
 * 在内存映射 I/O 中，内存与 I/O 设备共享相同的地址空间。
 * 在`QEMU`中，低地址用于 I/O 端口
 * 并且 DRAM 内存起始地址为`0x800000000`。
 * 因此我们将`DRAM_BASE`的值定为`0x800000000`。
 * 
 * - 由于内存映射 I/O，我们可以使用任何能够访问
 * 内存的指令来执行 I/O 操作。否则，我们将需要单独
 * 的指令和 I/O 端口的读写总线。
 * ```
 * 
 *                      +---------------+
 *                      |    Address    |
 *                      |  +---------+  |
 *   +-------+   addr   |  |   ROM   |  |
 *   |       |--------->|  +---------+  |
 *   |  CPU  |          |  |   RAM   |  |
 *   |       |<---------|  +---------+  |
 *   +-------+   data   |  |   I/O   |  |
 *                      |  +---------+  |
 *                      |               |
 *                      +---------------+
 * 
 * ```
 * 
 * ## DRAM 功能
 * - DRAM 是系统的内存。CPU 读取数据并从内存中取得指令，
 * 也向内存中存/写数据。因此 DRAM 有两个基本操作：
 * 从内存中读和写入内存。
 * 
 * - 定义函数`dram_load()`用于读取内存，
 * 以及`dram_store()`用于写入内存。
 */


#ifndef DRAM_H
#define DRAM_H

// ==================================================================== //
//                             Include
// ==================================================================== //


#include "typedef.h"

// ==================================================================== //
//                              Defines
// ==================================================================== //

#define DRAM_SIZE 1024*1024*1   /** 1MB 大小 DRAM */
#define DRAM_BASE 0x80000000    /** DRAM 基址 */




// ==================================================================== //
//                             Data: DRAM
// ==================================================================== //

/**
 * @brief DRAM 动态随机存取存储器结构体
 */
typedef struct DRAM_t {
    /** DRAM memory */
    // u8 mem[DRAM_SIZE];
    u8* mem_addr;  // 指向内存的地址指针
    size_t alloc_size;    // 已分配大小
    u8* alloc_addr; // 指向待分配地址的指针
} DRAM;




// ==================================================================== //
//                            Declare API: DRAM
// ==================================================================== //

/**
 * @brief 初始化DRAM结构体
 * @param dram 动态随机存取存储器
 */
void dram_init(DRAM* dram);

/**
 * @brief DRAM追加数据
 * @param dram 动态随机存取存储器
 * @param size 申请大小
 * @param data 数据
 */
void dram_alloc_data(DRAM* dram, size_t size, void* data);

/**
 * @brief DRAM向指定地址写入数据
 * @param dram 动态随机存取存储器
 * @param offset 偏移量
 * @param size 数据大小
 * @param value 数据值
 */
void dram_write_data(DRAM* dram, size_t offset, size_t size, u64 value);

/**
 * @brief DRAM在指定地址加载数据
 * @param dram 动态随机存取存储器
 * @param offset 偏移量
 * @param size 数据大小
 * @return u64 数据地址
 */
u64 dram_load_data(DRAM* dram, size_t offset, size_t size);

/**
 * @brief DRAM释放尾部数据
 * @param dram 动态随机存取存储器
 * @param size 数据大小
 */
void dram_release_data(DRAM* dram, size_t size);

/**
 * @brief 释放DRAM
 * @param dram 动态随机存取存储器
 */
void dram_free(DRAM* dram);

// ==================================================================== //
//                            Old API: DRAM
// ==================================================================== //

// /**
//  * @brief DRAM加载对应地址指定大小的数据
//  * @param dram DRAM结构体
//  * @param addr 地址
//  * @param size 数据大小
//  * @return u64 数据
//  */
// u64 dram_load(DRAM* dram, u64 addr, u64 size);

// /**
//  * @brief DRAM存取对应地址指定大小的数据
//  * @param dram DRAM结构体
//  * @param addr 地址
//  * @param size 数据大小
//  * @param value 数据
//  */
// void dram_store(DRAM* dram, u64 addr, u64 size, u64 value);

#endif  // DRAM_H