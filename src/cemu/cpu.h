/**
 * @file cpu.h
 * @author lancer (lancerstadium@163.com)
 * @brief 中央处理器头文件
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 * # CPU 介绍
 * 
 * ## CPU 取指与执行循环
 * 在这个实现中，我们有 3 级流水线用于执行所有指令，它们是：
 * 1. 第 1 级流水线（取指）：cpu 从 DRAM 的特定地址（存放在程序计数器 pc 中）中取得指令；
 * 2. 第 2 级流水线（译码）：指令被译码为操作码，目标寄存器与源寄存器等；
 * 3. 第 3 级流水线（执行）：此时指令按照译码后的结果在 ALU 中执行。
 * 
 */

#ifndef CPU_H
#define CPU_H

// ==================================================================== //
//                             Include
// ==================================================================== //

#include "bus.h"

// ==================================================================== //
//                             Data: CPU
// ==================================================================== //

/**
 * @brief 中央处理器结构体
 */
typedef struct CPU_t {
    u64 regs[32];           /** 32/64-bit 寄存器（x0-x31） */
    u64 pc;                 /** 64-bit 程序计数器 */
    u64 csr[4069];          /** 存储 CSR 指令 */
    BUS bus;                /** CPU连接总线 */
} CPU;

// ==================================================================== //
//                            Declare API: CPU
// ==================================================================== //

/**
 * @brief 处理器初始化给定的`CPU`，
 * 将指针指向的`CPU`中的寄存器全部置 0，
 * 并将程序寄存器`pc`的值设为内存的起始地址。
 * @param cpu 中央处理器
 */
void cpu_init(CPU *cpu);

/**
 * @brief 处理器从内存（DRAM）中读取指令用于执行，
 * 并将其存入指令变量`inst`中。
 * @param cpu 中央处理器
 * @return u32 32-bit 指令数据
 */
u32 cpu_fetch(CPU *cpu);

/**
 * @brief 处理器将从`DRAM`中取得并存放
 * 在`inst`变量中的指令解码并执行。本质上是 ALU 和指令译码器的组合。
 * @param cpu 中央处理器
 * @param inst 32-bit 指令数据
 * @return int 错误代码
 */
int cpu_execute(CPU *cpu, u32 inst);

/**
 * @brief 处理器步进执行
 * @param cpu 中央处理器
 * @param step 步数
 * @return int 错误代码
 */
int cpu_step(CPU* cpu, int step);

/**
 * @brief 处理器循环执行
 * @param cpu 中央处理器
 * @param filename 文件名
 * @return int 错误代码
 */
int cpu_loop(CPU* cpu, char* filename);

/**
 * @brief 处理器查看寄存器的值
 * @param cpu 中央处理器
 */
void cpu_dump_regs(CPU *cpu);


#endif // CPU_H