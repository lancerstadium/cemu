/**
 * @file main.c
 * @author lancer (you@domain.com)
 * @brief 程序入口
 * @version 0.1
 * @date 2023-12-24
 * @warning 此程序仅供学习
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "common.h"



/**
 * @brief 主函数
 * 
 * @param argc 参数个数
 * @param argv 参数值
 * @param envp 环境变量
 * @return int 
 */
int main(int argc, char *argv[], char *envp[])
{

    // test_i386();
    // test_riscv();
    arg_parse(argc, argv, envp);


    return 0;
}
