/**
 * @file macro.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MACRO_H
#define MACRO_H

#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#define MIN(a, b)   (((a) < (b)) ? (a) : (b))

// 定义一个宏，用于获取可变参数的第一个参数
#define FIRST_ARG(first, ...) first

// 定义一个宏，用于获取可变参数的剩余参数
#define REST_ARGS(first, ...) __VA_ARGS__


#endif // MACRO_H