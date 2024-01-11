/**
 * @file utils.c
 * @author lancer (lancerstadium@163.com)
 * @brief 工具头文件实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */

// ==================================================================== //
//                                Include
// ==================================================================== //

#include "utils.h"


// ==================================================================== //
//                            Func API: Utils
// ==================================================================== //

void arg_parser(int argc, char *argv[], char *envp[]) {
    // Step3: 初始化解析器
    ap_init_parser("uemu - a simple emulator", NULL);
    // Step4: 添加命令
    ap_add_command("default", "Cemu main func.", "cemu", default_callback, default_args);
    ap_add_command("hello", "Print `Hello, World!`.", "cemu hello", hello_callback, default_args);
    ap_add_command("debug", "Enter debug mode.", "This is usage.", debug_callback, debug_args);
    ap_add_command("test", "Unit test", "This is usage.", test_callback, test_args);
    // Step5: 开始解析
    ap_do_parser(argc, argv, envp);
}