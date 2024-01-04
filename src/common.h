/**
 * @file common.h
 * @author lancer (you@domain.com)
 * @brief 通用头文件
 * @version 0.1
 * @date 2023-12-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "utils.h"
#include "unicorn/unicorn.h"

/** 模拟执行基址 */
#define ADDRESS 0x1000000

/** 模拟执行代码：`INC ecx; DEC edx; PXOR xmm0, xmm1` */
#define X86_CODE32 "\x41\x4a\x66\x0f\xef\xc1"



/**
 * @brief hook 回调函数：在终端中输出起始地址和硬编码大小
 * 
 * @param uc `unicorn` 引擎句柄
 * @param address 指令地址
 * @param size 指令大小
 * @param user_data 用户数据
 * 
 * @note `uc` 为 `unicorn` 引擎句柄
 * @par 示例
 * ```c
 * hook_block(uc, 0x1000000, 0x100, NULL);
 * ```
 * 
 */
static void hook_block(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
    printf(">>> Tracing basic block at 0x%"PRIx64 ", block size = 0x%x\n", address, size);
}

/**
 * @brief hook 回调函数，用于监视程序运行时的变化
 * 
 * @param uc `unicorn` 引擎句柄
 * @param address 指令地址
 * @param size 指令大小
 * @param user_data 用户数据
 * 
 * @attention `uc` 为 `unicorn` 引擎句柄
 */
static void hook_code(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
    int eflags; /**> EFLAGS 寄存器值 */
    printf(">>> Tracing instruction at 0x%"PRIx64 ", instruction size = 0x%x\n", address, size);

    uc_reg_read(uc, UC_X86_REG_EFLAGS, &eflags); //获取寄存器值放入eflags变量中
    printf(">>> --- EFLAGS is 0x%x\n", eflags);

    // Uncomment below code to stop the emulation using uc_emu_stop()
    // if (address == 0x1000009)
    //    uc_emu_stop(uc);
}

/**
 * @brief 测试 x86 模拟执行环境
 * @note 功能还在开发中
 */
static void test_i386(void)
{
    uc_engine *uc;
    uc_err err;
    uint32_t tmp;
    uc_hook trace1, trace2;

    int r_ecx = 0x1234;     // ECX 寄存器
    int r_edx = 0x7890;     // EDX 寄存器
    // XMM0 、 XMM1 寄存器, 数组分别为低64位和高64位
    uint64_t r_xmm0[2] = {0x08090a0b0c0d0e0f, 0x0001020304050607};
    uint64_t r_xmm1[2] = {0x8090a0b0c0d0e0f0, 0x0010203040506070};

    printf("Emulate i386 code\n");

    // 初始化x86环境
    err = uc_open(UC_ARCH_X86, UC_MODE_32, &uc);
    if (err) {
        printf("Failed on uc_open() with error returned: %u\n", err);
        return;
    }

    // 为模拟执行代码申请 2MB 内存
    uc_mem_map(uc, ADDRESS, 2 * 1024 * 1024, UC_PROT_ALL);

    // 向目标地址写入opcode
    if (uc_mem_write(uc, ADDRESS, X86_CODE32, sizeof(X86_CODE32) - 1)) {
        printf("Failed to write emulation code to memory, quit!\n");
        return;
    }

    // 初始化寄存器ECX、EDX、XMM0、XMM1
    uc_reg_write(uc, UC_X86_REG_ECX, &r_ecx);
    uc_reg_write(uc, UC_X86_REG_EDX, &r_edx);
    uc_reg_write(uc, UC_X86_REG_XMM0, &r_xmm0);
    uc_reg_write(uc, UC_X86_REG_XMM1, &r_xmm1);

    // 在函数内插桩，成功时会调用回调函数 
    uc_hook_add(uc, &trace1, UC_HOOK_BLOCK, hook_block, NULL, 1, 0);

    // 每当代码执行时调用回调函数
    uc_hook_add(uc, &trace2, UC_HOOK_CODE, hook_code, NULL, 1, 0);

    // 模拟执行
    err = uc_emu_start(uc, ADDRESS, ADDRESS + sizeof(X86_CODE32) - 1, 0, 0);
    if (err) {
        printf("Failed on uc_emu_start() with error returned %u: %s\n",
                err, uc_strerror(err));
    }

    // 最后输出一些模拟执行完成后寄存器的值
    printf(">>> Emulation done. Below is the CPU context\n");

    uc_reg_read(uc, UC_X86_REG_ECX, &r_ecx);
    uc_reg_read(uc, UC_X86_REG_EDX, &r_edx);
    uc_reg_read(uc, UC_X86_REG_XMM0, &r_xmm0);
    printf(">>> ECX = 0x%x\n", r_ecx);
    printf(">>> EDX = 0x%x\n", r_edx);
    printf(">>> XMM0 = 0x%.16"PRIx64"%.16"PRIx64"\n", r_xmm0[1], r_xmm0[0]);

    // 读取内存中的内容
    if (!uc_mem_read(uc, ADDRESS, &tmp, sizeof(tmp)))
        printf(">>> Read 4 bytes from [0x%x] = 0x%x\n", ADDRESS, tmp);
    else
        printf(">>> Failed to read 4 bytes from [0x%x]\n", ADDRESS);

    // 最后需要关闭，否则会导致内存泄露
    uc_close(uc);
}


/**
 * @brief 测试 riscv 模拟执行环境
 * @warning 功能还在开发中
 * @par 示例
 * 
 * ## Code block
 * 
 * ```c
 * test_riscv();
 * ```
 */
void test_riscv() 
{
    printf("Emulate riscv code\n");
}