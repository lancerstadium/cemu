/**
 * @file argparse.h
 * @author lancer (you@domain.com)
 * @brief 命令行解析库头文件
 * @version 0.1
 * @date 2023-12-25
 *
 * @copyright Copyright (c) 2023
 * @note 参考项目：[Github | argparse](https://github.com/dwpeng/argparse)
 *
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color.h"

/* 解析器包含的子命令数 */
#ifndef ap_MAX_NCOMMAND
#define ap_MAX_NCOMMAND 10
#endif

/* 长参数名前的flag */
#ifndef ap_LONG_FLAG
#define ap_LONG_FLAG "--"
#endif

/* 短参数名前的flag */
#ifndef ap_SHORT_FLAG
#define ap_SHORT_FLAG "-"
#endif

#define ap_DEFAULT_COMMAND "default"

#define ap_END \
    {          \
        0      \
    }

#define ap_min(a, b) ((a) > (b) ? (b) : (a))
#define ap_max(a, b) ((a) > (b) ? (a) : (b))

#define NOW_CMD (&pp->commands[pp->command_pos])

/* 错误提示 */
#define ERROR_MSG _red(" [ERROR]")
#define ap_err(msg) (ERROR_MSG ": " msg "\n")

// 命令冲突
#ifndef ERROR_COMMAND_CONFLICT
#define ERROR_COMMAND_CONFLICT ap_err("Conflict.")
#endif

// 没有传递子命令
#ifndef ERROR_NO_SUBCOMMAND
#define ERROR_NO_SUBCOMMAND ap_err("Pass a subcommand.")
#endif

// 没有传递参数值
#ifndef ERROR_LOST_ARG_VALUE
#define ERROR_LOST_ARG_VALUE ap_err("\"%s\" lost arg value.")
#endif

#ifndef ERROR_DONOT_NEED_VALUE
#define ERROR_DONOT_NEED_VALUE ap_err("\"%s\" does not need arg value.")
#endif

#ifndef ERROR_ARG_NOT_EXIST
#define ERROR_ARG_NOT_EXIST ap_err("Arg name \"%s\" does not exist.")
#endif

#ifndef ERROR_LOST_ARG_NAME
#define ERROR_LOST_ARG_NAME ap_err("Except a arg name, but got \"%s\".")
#endif

#ifndef ERROR_SUBCOMMAND_NOT_EXIST
#define ERROR_SUBCOMMAND_NOT_EXIST ap_err("Subcommand %s does not exist.")
#endif

char *prog_name = NULL;              /* 程序名 */
char *prog_name_without_path = NULL; /* 程序名去除路径 */

/* 是否跟随参数 */
typedef enum
{
    ap_YES = 0,
    ap_NO
} ArgValue;

typedef struct
{
    /* 单短线参数名 */
    char *short_arg;
    /* 双短线参数名 */
    char *long_arg;
    /* 是否跟随参数值 */
    ArgValue arg_have_value;
    /* 传递值 */
    char *value;
    /* 初始值 */
    union
    {
        int i;
        bool b;
        float f;
        char *s;
        void *v;
    } init;
    /* 参数的说明 */
    char *help;
} ap_arg_t;

/* 定义参数用到的宏 */
#define ap_def_args(name) static ap_arg_t name[]
#define ap_def_callback(name) void name(int argc, char *argv[], char *envp[])

/* command对应的回调函数 */
typedef void (*callback_t)(int argc, char *argv[], char *envp[]);

typedef struct
{
    /* 命令名
     * - 全局命令为NULL
     * - 子命令时，为子命令名
     **/
    char *command;
    /* 对命令的描述 */
    char *description;
    /* 命令的用法 */
    char *usage;
    /* 是否是子命令 */
    int subcommand;
    /* 参数个数 */
    int nargs;
    /* 命令的回调函数 */
    callback_t callback;
    /* 存储的参数 */
    ap_arg_t *args;
} ap_command_t;

/* 打印command的回调函数 */
typedef void (*print_ap_command_t)(ap_command_t *);

typedef struct Parser Parser;
struct Parser
{
    /* 程序开始运行打印的内容
     * 可以是logo之类的
     **/
    char *print;
    /* 命令个数 */
    int ncommand;
    /* 存储的命令 */
    ap_command_t commands[ap_MAX_NCOMMAND];
    /* 当前命令行使用的command命令 */
    int command_pos;
    /* 自定义打印command参数 */
    print_ap_command_t print_command;
} p = {
    .ncommand = 0,
    .command_pos = 0},
  *pp = &p;

static int have_global = 0;
static int have_subcommand = 0;

/**
 * @brief 初始化解析器
 *
 * @param print_message
 * @param print_command
 */
static inline void
ap_init_parser(char *print_message, print_ap_command_t print_command)
{
    pp->print = print_message;
    if (print_command != NULL)
    {
        pp->print_command = print_command;
    }
}

/**
 * @brief 添加一个子命令
 *
 * @param command
 * @param description
 * @param usage
 * @param callback
 * @param args
 */
static inline void
ap_add_command(
    char *command,
    char *description,
    char *usage,
    callback_t callback,
    ap_arg_t *args)
{
    if (command == NULL)
    {
        have_global = 1;
    }
    else if (command == ap_DEFAULT_COMMAND)
    {
        have_global = 1;
        have_subcommand = 1;
    }
    else
    {
        have_subcommand = 1;
    }
    /* 全局命令和子命令有且只能存在一种
     * 要么有一个全局命令
     * 要么存在多个子命令
     **/

    // if (have_global && have_subcommand)
    // {
    //     fprintf(stderr, ERROR_COMMAND_CONFLICT);
    //     exit(1);
    // }
    if (!(have_global || have_subcommand))
    {
        fprintf(stderr, ERROR_NO_SUBCOMMAND);
        exit(1);
    }

    if (pp->ncommand > ap_MAX_NCOMMAND - 1)
    {
        fprintf(stderr, _red(" [ERROR]:") "Too many commands. Change ap_MAX_NCOMMAND bigger.\n");
        exit(1);
    }

    pp->commands[pp->ncommand].command = command;
    pp->commands[pp->ncommand].description = (char *)malloc(strlen(description) + 1);
    strcpy(pp->commands[pp->ncommand].description, description == NULL ? "" : description);
    pp->commands[pp->ncommand].usage = (char *)malloc(strlen(usage) + 1);
    strcpy(pp->commands[pp->ncommand].usage, usage == NULL ? "" : usage);
    pp->commands[pp->ncommand].callback = callback;
    pp->commands[pp->ncommand].args = args;
    // 统计当前command有多少参数
    int nargs = 0;
    while (1)
    {
        if (args[nargs].long_arg || args[nargs].short_arg)
        {
            if (args[nargs].help == NULL)
            {
                args[nargs].help = "";
            }
            nargs++;
        }
        else
        {
            break;
        }
    }
    pp->commands[pp->ncommand].nargs = nargs;
    pp->ncommand++;
}

/**
 * @brief 判断当前参数名是否与command中的参数名相同
 *
 * @param arg_name
 * @param arg
 * @return int
 */
static inline int
_is_eq(char *arg_name, ap_arg_t arg)
{
    char *short_arg = arg.short_arg;
    char *long_arg = arg.long_arg;
    if (strcmp(arg_name, long_arg) == 0 || strcmp(arg_name, short_arg) == 0)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief 根据参数名获取参数值
 *
 * @param arg_name
 * @return void*
 */
static inline ap_arg_t *
ap_get(char *arg_name)
{
    if ((NOW_CMD) == NULL)
    {
        return NULL;
    }

    ap_arg_t *p = NULL;
    int nargs = (NOW_CMD)->nargs;
    ap_arg_t *args = (NOW_CMD)->args;
    for (int i = 0; i < nargs; i++)
    {
        if (_is_eq(arg_name, args[i]))
        {
            p = &args[i];
            break;
        }
    }
    return p;
}

/**
 * @brief 打印command
 *
 * @param c
 */
static inline void
ap_default_print_command(ap_command_t *c)
{
    fprintf(stderr, "\n> " _bold("%s ") _bgreen("%s") _iblack(" < ... >\n   ") _ubold("Descr:") _iblack("  %s"), prog_name_without_path, c->command, c->description);
    fprintf(stderr, "\n   " _ubold("Usage:") _iblack("  %s\n"), c->usage);
    for (size_t i = 0; i < c->nargs; i++)
    {
        fprintf(
            stderr,
            "       " _red("%s%s") "  %s%-10s" _iblack("%s\n"),
            ap_SHORT_FLAG,
            c->args[i].short_arg,
            ap_LONG_FLAG,
            c->args[i].long_arg,
            c->args[i].help);
    }
    fprintf(stderr, "\n");
}

/**
 * @brief 打印 command 简易版
 *
 * @param c
 */
static inline void
ap_default_print_base_command(ap_command_t *c)
{
    fprintf(stderr, _bgreen(" %8s") _iblack("  %s\n"), c->command, c->description);

    for (size_t i = 0; i < c->nargs; i++)
    {
        if (i >= 2)
        {
            fprintf(
                stderr,
                "       " _red("%s%s") "  %s%-10s" _iblack("%s\n"),
                ap_SHORT_FLAG,
                "h",
                ap_LONG_FLAG,
                "help",
                "Get more info ...");
            break;
        }
        fprintf(
            stderr,
            "       " _red("%s%s") "  %s%-10s" _iblack("%s\n"),
            ap_SHORT_FLAG,
            c->args[i].short_arg,
            ap_LONG_FLAG,
            c->args[i].long_arg,
            c->args[i].help);
    }
    // fprintf(stderr, "\n");
}

/**
 * @brief 打印解析器
 *
 */
static inline void
ap_print_parser(void)
{
    fprintf(stderr, "\n %s\n ", pp->print);
    if (pp->ncommand > 1)
    {
        fprintf(stderr, _ubold("Command:\n"));
        for (size_t i = 0; i < pp->ncommand; i++)
        {
            ap_default_print_base_command(&pp->commands[i]);
        }
    }
    else
    {
        pp->print_command(NOW_CMD);
    }

    fprintf(stderr, "\n");
}

static inline void
ap_print_command(void)
{
    if (!pp->print_command)
    {
        ap_default_print_command(NOW_CMD);
    }
    else
    {
        pp->print_command(NOW_CMD);
    }
}

/**
 * @brief 解析命令行，内部接口
 *
 * @param argc
 * @param argv
 */
static inline void
_ap_parser_command_line(int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0)
        {
            ap_print_command();
            exit(0);
        }
    }
    // 当前是参数名状态，还是参数值状态
    // 为0为参数名状态
    // 为1为参数值状态
    int status = 0;

    // 解析的位置
    int count = 0;

    // 参数名的前缀长度
    int short_flag_len = strlen(ap_SHORT_FLAG);
    int long_flag_len = strlen(ap_LONG_FLAG);

    // 准备装载的参数
    ap_arg_t *arg = NULL;
    char *arg_name = NULL;
    for (;;)
    {
        if (count >= argc)
        {
            break;
        }

        // 解析器的位置
        char *need_parse = argv[count];
        int need_parse_len = strlen(need_parse);

        // 判断是否为一个参数名
        int is_short = strncmp(need_parse, ap_SHORT_FLAG, ap_min(need_parse_len, short_flag_len)) == 0;
        int is_long = strncmp(need_parse, ap_LONG_FLAG, ap_min(need_parse_len, long_flag_len)) == 0;

        if (is_short || is_long)
        {
            status = 0;
            // 去掉参数名前面的prefix
            const char *prefix;
            if (is_long)
            {
                prefix = ap_LONG_FLAG;
            }
            else
            {
                prefix = ap_SHORT_FLAG;
            }
            int prefix_len = strlen(prefix);
            arg_name = need_parse + prefix_len;

            // 检查参数是否存在
            int exist = 0;
            for (int i = 0; i < (NOW_CMD)->nargs; i++)
            {
                arg = &((NOW_CMD)->args[i]);
                int short_succ = strcmp(arg_name, arg->short_arg) == 0;
                int long_succ = strcmp(arg_name, arg->long_arg) == 0;
                if (short_succ || long_succ)
                {
                    exist = 1;
                    break;
                }
            }
            if (!exist)
            {
                fprintf(stderr, ERROR_ARG_NOT_EXIST, arg_name - prefix_len);
                exit(1);
            }
        }
        else
        {
            // 参数值/一个位置参数
            status = 1;
        }

        if (arg == NULL)
        {
            fprintf(stderr, ERROR_LOST_ARG_NAME, need_parse);
            exit(1);
        }

        // 根据status来进行下一步的处理
        if (status && arg->arg_have_value == ap_NO)
        {
            // 不需要参数值
            fprintf(stderr, ERROR_DONOT_NEED_VALUE, arg->long_arg);
            exit(1);
        }

        if (status && arg->arg_have_value == ap_YES)
        {
            arg->value = need_parse;
            arg = NULL;
        }

        if (!status && arg->arg_have_value == ap_NO)
        {
            arg->init.b = 1;
        }
        count++;
    }
    if (arg && arg->arg_have_value == ap_YES)
    {
        fprintf(stderr, ERROR_LOST_ARG_VALUE, arg->long_arg);
        exit(1);
    }
}

/**
 * @brief 解析命令行
 *
 * @param argc
 * @param argv
 */
static inline void
ap_do_parser(int argc, char *argv[], char *envp[])
{
    int argc_copy = argc;
    char **argv_copy = argv;
    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            ap_print_parser();
            exit(0);
        }
    }

    prog_name = argv[0];
    prog_name_without_path = strrchr(prog_name, '/');
    if (prog_name_without_path)
    {
        prog_name_without_path++;
    }
    else
    {
        prog_name_without_path = prog_name;
    }

    // 跳过文件名
    argc--;
    argv++;
    if (have_subcommand)
    {
        char *subcommand;
        int exist = 0;
        if (!argc && !have_global)
        { // 子命令
            ap_print_parser();
            fprintf(stderr, ERROR_NO_SUBCOMMAND);
            exit(1);
        }
        else if (!argc && have_global)
        { // 全局命令
            subcommand = ap_DEFAULT_COMMAND;
        }
        else
        {
            subcommand = argv[0];
        }
        /* 在这里自动装载子命令 */
        for (int i = 0; i < pp->ncommand; i++)
        {
            if (strcmp(pp->commands[i].command, subcommand) == 0)
            {
                pp->commands[i].subcommand = 1;
                pp->command_pos = i;
                exist = 1;
                break;
            }
        }
        if (!exist)
        {
            fprintf(stderr, ERROR_SUBCOMMAND_NOT_EXIST, subcommand);
            exit(1);
        }
    }
    else
    {
        // 只有全局命令
        pp->command_pos = 0;
    }

    /* 开始解析命令行参数 */
    _ap_parser_command_line(argc - have_subcommand, argv + have_subcommand);
    /* 开始调用回调函数 */
    (NOW_CMD)->callback(argc_copy, argv_copy, envp);
}

#endif
