/**
 * @file cemu.h
 * @author lancer (lancerstadium@163.com)
 * @brief cemu通用头文件
 * @version 0.1
 * @date 2023-12-24
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef CEMU_H
#define CEMU_H

// ==================================================================== //
//                                Include
// ==================================================================== //

#include "cpu.h"
#include "loader.h"
#include "utils.h"

// 测试
void run_unit_test() {
    // test
    ut_print_test();
}

// ==================================================================== //
//                        Monitor Command Callback
// ==================================================================== //

void help_command_callback() {
    printf("help info\n");
}

void run_command_callback(char *args, CPU *cpu) {
    cpu_step(cpu, -1);
}

void step_command_callback(char *args, CPU *cpu) {
    if(args != NULL && strlen(args) > 0){
        if(!cpu_step(cpu, atoi(args))) 
            log_error("CPU step error!");
    }else {
        log_warn("step one");
        if(!cpu_step(cpu, 1)) 
            log_error("CPU step error!");
    }
}

// ==================================================================== //
//                        Argparse Command Callback
// ==================================================================== //

ap_def_callback(default_callback) {
    if(argc <= 1) {
        log_error("No input file");
        exit(-1);
    }
    CPU cpu;
    cpu_loop(&cpu, ap_get("input")->value);
}

ap_def_callback(hello_callback) {
    log_debug("Hello, World!");
    log_trace("Hello, World!");
    log_info("Hello, World!");
    log_warn("Hello, World!");
    log_error("Hello, World!");
    log_assert(5 == 5, "Hello, World!");
}

ap_def_callback(test_callback) {
    ap_arg_t* arg_o = ap_get("output");
    log_assert(arg_o != NULL, "arg_o not NULL");
    log_assert(arg_o->value == NULL, "");

    if (!arg_o->value) {
        printf("no value. init: %s\n", arg_o->init.s);
    } else {
        printf("option output: %s\n", arg_o->value);
    }
    int q_l;
    if(!ap_get("quiet")->value) {
        q_l = ap_get("quiet")->init.i;
    } else {
        q_l = atoi(ap_get("quiet")->value);
    }
    printf("set quiet: %d\n", q_l);
    ut_set_quiet(q_l);
    run_unit_test();
}

ap_def_callback(debug_callback) {

    CPU cpu;
    // 1. 初始化：cpu, 寄存器 regs 和程序计数器 pc
    cpu_init(&cpu);
    // 2. 加载文件
    if(!ap_get("input")->value) {
        char* default_input = "./test/temp_02.out";
        log_warn("No input file, use: %s", default_input);
        load_elf(&cpu, default_input);
    } else {
        load_elf(&cpu, ap_get("input")->value);
    }
    linenoiseHistoryLoad("history.txt"); // Load command history from file
    char *line;
    while ((line = linenoise(">> ")) != NULL) {
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line); // Add command to history
            linenoiseHistorySave("history.txt"); // Save command history to file
            char *command = strtok(line, " "); // 解析命令
            char *args = strtok(NULL, " "); // 解析参数
            if (strcmp(command, "help") == 0 || strcmp(command, "h" ) == 0) {
                help_command_callback();
            } else if (strcmp(command, "run" ) == 0 || strcmp(command, "r" ) == 0) {
                run_command_callback(args, &cpu);
            } else if (strcmp(command, "step") == 0 || strcmp(command, "si") == 0) {
                step_command_callback(args, &cpu);
            } else if (strcmp(command, "load") == 0 || strcmp(command, "l" ) == 0) {
                load_elf(&cpu, args);
            } else if (strcmp(command, "quit") == 0 || strcmp(command, "q" ) == 0) {
                free(line);
                log_info("Bye!");
                exit(0);
            } else {
                log_warn("Unknown command: %s", command);
            }
            free(line);
        }
    }
}


#endif  // CEMU_H