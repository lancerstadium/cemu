/**
 * @file loader.h
 * @author lancer (lancerstadium@163.com)
 * @brief 文件加载头文件
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef LOADER_H
#define LOADER_H


// ==================================================================== //
//                               Include
// ==================================================================== //

#include "cpu.h"


// ==================================================================== //
//                            Declare API: CPU
// ==================================================================== //

/**
 * @brief 加载文件到处理器
 * @param cpu 中央处理器
 * @param filename 文件名
 */
void load_file(CPU* cpu, char* filename);

void load_elf(CPU* cpu, char* filename);

#endif // LOADER_H