/**
 * @file utils.h
 * @author lancer (lancerstadium@163.com)
 * @brief 工具头文件
 * @version 0.1
 * @date 2023-12-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef UTILS_H
#define UTILS_H


// ==================================================================== //
//                                Include
// ==================================================================== //

#include "log.h"
// #include "argparse-new.h"
#include "argparse.h"
#include "linenoise.h"
#include "unittest.h"
#include "typedef.h"
#include "macro.h"
// #include "list.h"
// #include "obj.h"


// ==================================================================== //
//                                Data: Args def
// ==================================================================== //

// Step1: 定义参数
ap_def_args(default_args) = {
    {.short_arg = "o", .long_arg = "output", .init.s = "./a.out", .help = "set output path"},
    {.short_arg = "q", .long_arg = "quiet",  .init.i = 3, .help = "set quiet level"},
    AP_INPUT_ARG,
    AP_END_ARG};

ap_def_args(test_args) = {
    {.short_arg = "o", .long_arg = "output", .init.s = "./test", .help = "set output path"},
    {.short_arg = "q", .long_arg = "quiet",  .init.i = 3, .help = "set quiet level"},
    AP_END_ARG};

ap_def_args(debug_args) = {
    {.short_arg = "o", .long_arg = "output", .init.s = "./test", .help = "set output path"},
    {.short_arg = "l", .long_arg = "log", .init.s = "./log", .help = "set log file"},
    AP_INPUT_ARG,
    AP_END_ARG};



// ==================================================================== //
//                            Declare API: Utils
// ==================================================================== //

void run_unit_test();

// Step2: 定义回调函数
ap_def_callback(default_callback);
ap_def_callback(hello_callback);
ap_def_callback(debug_callback);
ap_def_callback(test_callback);

/**
 * @brief 参数解析
 * @param argc 参数个数
 * @param argv 参数值
 * @param envp 环境变量
 */
void arg_parser(int argc, char *argv[], char *envp[]);


#endif /** UTILS_H */
