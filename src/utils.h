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

#include "log.h"
#include "argparse.h"
#include "linenoise.h"
#include "unittest.h"
#include "obj.h"

// Step1: 定义参数
ap_def_args(help_args) = {
    {.short_arg = "a", .long_arg = "apple", .init.s = "default_value", .help = "set apple value"},
    {.short_arg = "s", .long_arg = "sleep", .arg_have_value = ap_NO, .init.b = false},
    ap_END};

ap_def_args(test_args) = {
    {.short_arg = "o", .long_arg = "output", .init.s = "./test", .help = "set output path"},
    {.short_arg = "q", .long_arg = "quiet", .arg_have_value = ap_NO, .init.b = false, .help = "quiet run"},
    ap_END};

ap_def_args(debug_args) = {
    {.short_arg = "o", .long_arg = "output", .init.s = "./test", .help = "set output path"},
    {.short_arg = "l", .long_arg = "log", .init.s = "./log", .help = "set log file"},
    ap_END};

// Step2: 定义回调函数
ap_def_callback(help_callback)
{
    ap_arg_t *arg = ap_get("apple");
    if (!arg->value)
    {
        printf("no value. init: %s\n", arg->init.s);
    }
    else
    {
        printf("%s\n", arg->value);
    }
    printf("%d\n", ap_get("sleep")->init.b);
}

// Class declaration
classdef(Hello);
class(Hello, 
    public(
        int (*get_no)(); 
        void (*print_msg)();
    ), 
    private(
        int no; 
        char *msg;
    )
);

ctor(Hello)(int no);              // Constructor
dtor(Hello);                      // Destructor (optional)
method(Hello, int, get_no)();     // Method
method(Hello, void, print_msg)(); // Method

ctor(Hello)(int no)
{
    obj_setup(Hello);
    obj_bind(Hello, get_no);
    obj_bind(Hello, print_msg);
    self->no = no;
    self->msg = "Hello, world!\n";
    obj_done(Hello);
}

method(Hello, int, get_no)()
{
    obj_prepare(Hello);
    return self->no;
}

method(Hello, void, print_msg)()
{
    obj_prepare(Hello);
    printf("%s", self->msg);
}

ap_def_callback(hello_callback)
{
    Hello hl = new (Hello)(15);
    printf("get no: %d\n", hl->get_no());
    hl->print_msg();
    log_debug("Hello, World!");
    log_trace("Hello, World!");
    log_info("Hello, World!");
    log_warn("Hello, World!");
    log_error("Hello, World!");
}


ut_dec_test(one);
ut_dec_test(two);
ut_dec_test(byte_and_int);
ut_dec_test(float_suit);
ut_dec_test(big_endian);

ap_def_callback(test_callback)
{
    ap_arg_t *arg = ap_get("output");
    if (!arg->value)
    {
        printf("no value. init: %s\n", arg->init.s);
    }
    else
    {
        printf("option output: %s\n", arg->value);
    }
    printf("option quiet: %d\n", ap_get("quiet")->init.b);
    // test
    if (!ap_get("quiet")->init.b)
    {
        ut_set_quiet(3);
        ut_run_test(one);
        ut_run_test(two);
        ut_run_test(byte_and_int);
        ut_run_test(float_suit);
        ut_run_test(big_endian);
        ut_print_test();
    }
}

ap_def_callback(debug_callback)
{
    char *line;
    while ((line = linenoise("hello> ")) != NULL)
    {
        printf("You wrote: %s\n", line);
        linenoiseFree(line); /* Or just free(line) if you use libc malloc. */
    }
}

/**
 * @brief 参数解析
 *
 * @param argc 参数个数
 * @param argv 参数值
 * @param envp 环境变量
 * @note
 * # 参数解析步骤
 * 1. Step1: 使用宏 `ap_def_args` 定义参数
 * ```c
 * ap_def_args(test_args) = {
 *   {.short_arg = "o", .long_arg = "output", .init.s = "./test", .help = "set output path"},
 *   {.short_arg = "q", .long_arg = "quiet" , .init.b = true    , .help = "quiet run"},
 *   {.short_arg = "d", .long_arg = "debug" , .init.b = false   , .help = "debug mode"},
 *   ap_END
 * };
 * ```
 *
 * 2. Step2: 使用宏 `ap_def_callback` 定义回调函数
 * ```c
 * ap_def_callback(test_callback) {
 *   ap_arg_t *arg = ap_get("output");
 *   if (!arg->value) {
 *     printf("no value. init: %s\n", arg->init.s);
 *   }
 *   else {
 *     printf("option output: %s\n", arg->value);
 *   }
 *   printf("option quiet: %d\n", ap_get("quiet")->init.b);
 *   printf("option debug: %d\n", ap_get("debug")->init.b);
 * }
 * ```
 *
 * 3. Step3: 使用宏 `ap_init_parser` 初始化解析器
 * ```c
 * ap_init_parser("uemu - a simple emulator", NULL);
 * ```
 *
 * 4. Step4: 使用宏 `ap_add_command` 添加命令
 * ```c
 * ap_add_command("test", "Print `Hello, World!`.", "This is usage.", test_callback, test_args);
 * ```
 * @attention 添加命令名为 `default` 时，是设置主命令：
 *
 * 5. Step5: 使用宏 `ap_add_command` 解析命令
 * ```c
 * ap_do_parser(argc, argv, envp);
 * ```
 */
void arg_parse(int argc, char *argv[], char *envp[])
{
    // Step3: 初始化解析器
    ap_init_parser("uemu - a simple emulator", NULL);
    // Step4: 添加命令
    ap_add_command("default", "Default: print `Hello, World!`", "uemu test", hello_callback, help_args);
    ap_add_command("help", "This is description.", "This is usage.", help_callback, help_args);
    ap_add_command("debug", "Enter debug mode.", "This is usage.", debug_callback, debug_args);
    ap_add_command("test", "Unit test", "This is usage.", test_callback, test_args);
    // Step5: 开始解析
    ap_do_parser(argc, argv, envp);
}

#endif /** UTILS_H */
