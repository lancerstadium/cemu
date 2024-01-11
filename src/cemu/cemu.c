/**
 * @file cemu.c
 * @author lancer (lancerstadium@163.com)
 * @brief cemu总程序入口
 * @version 0.1
 * @date 2023-12-24
 * @copyright Copyright (c) 2023
 * 
 */


// ==================================================================== //
//                               Include
// ==================================================================== //

#include "cemu.h"


// ==================================================================== //
//                            Main Proc Entry
// ==================================================================== //

/**
 * @brief 主函数
 * @param argc 参数个数
 * @param argv 参数值
 * @param envp 环境变量
 * @return int 
 */
int main(int argc, char *argv[], char *envp[]) {
    arg_parser(argc, argv, envp);
    return 0;
}
