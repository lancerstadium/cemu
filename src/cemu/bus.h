/**
 * @file bus.h
 * @author lancer (lancerstadium@163.com)
 * @brief 总线头文件
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 * # 总线介绍
 * - 总线只是为计算机提供了一条各个组件之间数据传输的路径。
 * 对于的 RISC-V 模拟器，地址总线与数据总线是单一的 64 位
 * 宽总线（对于 64 位实现）。
 * - 本例中的总线连接 CPU 与 DRAM。因此我们编写的总线结构
 * 有一个 DRAM 对象，表示我们要连接到的 DRAM。
 */


#ifndef BUS_H
#define BUS_H

// ==================================================================== //
//                             Include
// ==================================================================== //


#include "mmu.h"

// ==================================================================== //
//                             Data: BUS
// ==================================================================== //

typedef struct BUS_t {
    DRAM dram;          /** 动态随机存取存储器 */
} BUS;


// ==================================================================== //
//                            Declare API: BUS
// ==================================================================== //

/**
 * @brief 总线加载数据
 * @param bus 总线
 * @param addr 地址
 * @param size 数据大小
 * @return u64 数据
 */
u64 bus_load(BUS* bus, u64 addr, u64 size);

/**
 * @brief 总线存储数据
 * @param bus 总线
 * @param addr 地址
 * @param size 数据大小
 * @param value 数据
 */
void bus_store(BUS* bus, u64 addr, u64 size, u64 value);


#endif // BUS_H