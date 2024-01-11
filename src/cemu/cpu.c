/**
 * @file cpu.c
 * @author lancer (lancerstadium@163.com)
 * @brief 中央处理器实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * @note 当编写 exec 函数时，应当注意 0 扩展与符号扩展。
 * 例如，立即数在与寄存器中的其它值一起被操作时，
 * 通常是符号扩展到 64 位。我们可以在需要时通过
 *  C 语言的类型转换（int32_t) > (int64_t) > (u64) 
 * 来实现符号位扩展。
 */

// ==================================================================== //
//                                Include
// ==================================================================== //

#include "csr.h"
#include "opcode.h"
#include "utils.h"

// ==================================================================== //
//                                Define
// ==================================================================== //

#define ADDR_MISALIGNED(addr) (addr & 0x3)
#define MAX_CPU_STEP 10

// ==================================================================== //
//                            Private Func: CPU
// ==================================================================== //

/**
 * @brief *处理器加载数据
 * @param cpu 中央处理器
 * @param addr 地址
 * @param size 数据大小
 * @return u64 数据
 */
static inline u64 cpu_load(CPU* cpu, u64 addr, u64 size) {
    return bus_load(&(cpu->bus), addr, size);
}

/**
 * @brief *处理器存储数据
 * @param cpu 中央处理器
 * @param addr 地址
 * @param size 数据大小
 * @param value 数据
 */
static inline void cpu_store(CPU* cpu, u64 addr, u64 size, u64 value) {
    bus_store(&(cpu->bus), addr, size, value);
}

/**
 * @brief 打印操作码
 * @param s 操作码字符串
 */
static void print_op(char* s) {
    printf(_blue("%s"), s);
}

/**
 * @brief 处理器执行一条指令
 * @param cpu 中央处理器
 * @return int 错误代码
 */
static int cpu_step_one(CPU* cpu) {
    // 取指令 fetch
    u32 inst = cpu_fetch(cpu);
    // 增长程序计数器
    cpu->pc += 4;
    // 指令执行
    return cpu_execute(cpu, inst);
}

// ==================================================================== //
//                            CPU Decode
// ==================================================================== //

static inline u64 rd(u32 inst) {
    return (inst >> 7) & 0x1f;    // rd in bits 11..7
}
static inline u64 rs1(u32 inst) {
    return (inst >> 15) & 0x1f;   // rs1 in bits 19..15
}
static inline u64 rs2(u32 inst) {
    return (inst >> 20) & 0x1f;   // rs2 in bits 24..20
}
static inline u64 imm_I(u32 inst) {
    // imm[11:0] = inst[31:20]
    return ((int64_t)(int32_t) (inst & 0xfff00000)) >> 20;
}
static inline u64 imm_S(u32 inst) {
    // imm[11:5] = inst[31:25], imm[4:0] = inst[11:7]
    return ((int64_t)(int32_t)(inst & 0xfe000000) >> 20)
        | ((inst >> 7) & 0x1f);
}
static inline u64 imm_B(u32 inst) {
    // imm[12|10:5|4:1|11] = inst[31|30:25|11:8|7]
    return ((int64_t)(int32_t)(inst & 0x80000000) >> 19)
        | ((inst & 0x80) << 4) // imm[11]
        | ((inst >> 20) & 0x7e0) // imm[10:5]
        | ((inst >> 7) & 0x1e); // imm[4:1]
}
static inline u64 imm_U(u32 inst) {
    // imm[31:12] = inst[31:12]
    return (int64_t)(int32_t)(inst & 0xfffff999);
}
static inline u64 imm_J(u32 inst) {
    // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
    return (u64)((int64_t)(int32_t)(inst & 0x80000000) >> 11)
        | (inst & 0xff000) // imm[19:12]
        | ((inst >> 9) & 0x800) // imm[11]
        | ((inst >> 20) & 0x7fe); // imm[10:1]
}
static inline u32 shamt(u32 inst) {
    // shamt(shift amount) only required for immediate shift instructions
    // shamt[4:5] = imm[5:0]
    return (u32) (imm_I(inst) & 0x1f); // TODO: 0x1f / 0x3f ?
}

static inline u64 csr(u32 inst) {
    // csr[11:0] = inst[31:20]
    return ((inst & 0xfff00000) >> 20);
}

// ==================================================================== //
//                       CPU Inst Exec: U-type
// ==================================================================== //

void exec_LUI(CPU* cpu, u32 inst) {
    // LUI places upper 20 bits of U-immediate value to rd
    cpu->regs[rd(inst)] = (u64)(int64_t)(int32_t)(inst & 0xfffff000);
    print_op("lui\n");
}

void exec_AUIPC(CPU* cpu, u32 inst) {
    // AUIPC forms a 32-bit offset from the 20 upper bits 
    // of the U-immediate
    u64 imm = imm_U(inst);
    cpu->regs[rd(inst)] = ((int64_t) cpu->pc + (int64_t) imm) - 4;
    print_op("auipc\n");
}

void exec_JAL(CPU* cpu, u32 inst) {
    u64 imm = imm_J(inst);
    cpu->regs[rd(inst)] = cpu->pc;
    /*print_op("JAL-> rd:%ld, pc:%lx\n", rd(inst), cpu->pc);*/
    cpu->pc = cpu->pc + (int64_t) imm - 4;
    print_op("jal\n");
    if (ADDR_MISALIGNED(cpu->pc)) {
        fprintf(stderr, "JAL pc address misalligned");
        exit(0);
    }
}

void exec_JALR(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    u64 tmp = cpu->pc;
    cpu->pc = (cpu->regs[rs1(inst)] + (int64_t) imm) & 0xfffffffe;
    cpu->regs[rd(inst)] = tmp;
    /*print_op("NEXT -> %#lx, imm:%#lx\n", cpu->pc, imm);*/
    print_op("jalr\n");
    if (ADDR_MISALIGNED(cpu->pc)) {
        fprintf(stderr, "JAL pc address misalligned");
        exit(0);
    }
}

void exec_BEQ(CPU* cpu, u32 inst) {
    u64 imm = imm_B(inst);
    if ((int64_t) cpu->regs[rs1(inst)] == (int64_t) cpu->regs[rs2(inst)])
        cpu->pc = cpu->pc + (int64_t) imm - 4;
    print_op("beq\n");
}
void exec_BNE(CPU* cpu, u32 inst) {
    u64 imm = imm_B(inst);
    if ((int64_t) cpu->regs[rs1(inst)] != (int64_t) cpu->regs[rs2(inst)])
        cpu->pc = (cpu->pc + (int64_t) imm - 4);
    print_op("bne\n");
}
void exec_BLT(CPU* cpu, u32 inst) {
    /*print_op("Operation: BLT\n");*/
    u64 imm = imm_B(inst);
    if ((int64_t) cpu->regs[rs1(inst)] < (int64_t) cpu->regs[rs2(inst)])
        cpu->pc = cpu->pc + (int64_t) imm - 4;
    print_op("blt\n");
}
void exec_BGE(CPU* cpu, u32 inst) {
    u64 imm = imm_B(inst);
    if ((int64_t) cpu->regs[rs1(inst)] >= (int64_t) cpu->regs[rs2(inst)])
        cpu->pc = cpu->pc + (int64_t) imm - 4;
    print_op("bge\n");
}
void exec_BLTU(CPU* cpu, u32 inst) {
    u64 imm = imm_B(inst);
    if (cpu->regs[rs1(inst)] < cpu->regs[rs2(inst)])
        cpu->pc = cpu->pc + (int64_t) imm - 4;
    print_op("bltu\n");
}
void exec_BGEU(CPU* cpu, u32 inst) {
    u64 imm = imm_B(inst);
    if (cpu->regs[rs1(inst)] >= cpu->regs[rs2(inst)])
        cpu->pc = (int64_t) cpu->pc + (int64_t) imm - 4;
    print_op("jal\n");
}
void exec_LB(CPU* cpu, u32 inst) {
    // load 1 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = (int64_t)(int8_t) cpu_load(cpu, addr, 8);
    print_op("lb\n");
}
void exec_LH(CPU* cpu, u32 inst) {
    // load 2 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = (int64_t)(int16_t) cpu_load(cpu, addr, 16);
    print_op("lh\n");
}
void exec_LW(CPU* cpu, u32 inst) {
    // load 4 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = (int64_t)(int32_t) cpu_load(cpu, addr, 32);
    print_op("lw\n");
}
void exec_LD(CPU* cpu, u32 inst) {
    // load 8 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = (int64_t) cpu_load(cpu, addr, 64);
    print_op("ld\n");
}
void exec_LBU(CPU* cpu, u32 inst) {
    // load unsigned 1 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = cpu_load(cpu, addr, 8);
    print_op("lbu\n");
}
void exec_LHU(CPU* cpu, u32 inst) {
    // load unsigned 2 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = cpu_load(cpu, addr, 16);
    print_op("lhu\n");
}
void exec_LWU(CPU* cpu, u32 inst) {
    // load unsigned 2 byte to rd from address in rs1
    u64 imm = imm_I(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu->regs[rd(inst)] = cpu_load(cpu, addr, 32);
    print_op("lwu\n");
}
void exec_SB(CPU* cpu, u32 inst) {
    u64 imm = imm_S(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu_store(cpu, addr, 8, cpu->regs[rs2(inst)]);
    print_op("sb\n");
}
void exec_SH(CPU* cpu, u32 inst) {
    u64 imm = imm_S(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu_store(cpu, addr, 16, cpu->regs[rs2(inst)]);
    print_op("sh\n");
}
void exec_SW(CPU* cpu, u32 inst) {
    u64 imm = imm_S(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu_store(cpu, addr, 32, cpu->regs[rs2(inst)]);
    print_op("sw\n");
}
void exec_SD(CPU* cpu, u32 inst) {
    u64 imm = imm_S(inst);
    u64 addr = cpu->regs[rs1(inst)] + (int64_t) imm;
    cpu_store(cpu, addr, 64, cpu->regs[rs2(inst)]);
    print_op("sd\n");
}

// ==================================================================== //
//                       CPU Inst Exec: I-type
// ==================================================================== //

/**
 * @brief 立即数相加
 * @param cpu 处理器
 * @param inst 指令
 */
void exec_ADDI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] + (int64_t) imm;
    print_op("addi\n");
}

void exec_SLLI(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] << shamt(inst);
    print_op("slli\n");
}

void exec_SLTI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < (int64_t) imm)?1:0;
    print_op("slti\n");
}

void exec_SLTIU(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < imm)?1:0;
    print_op("sltiu\n");
}

void exec_XORI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] ^ imm;
    print_op("xori\n");
}

void exec_SRLI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] >> imm;
    print_op("srli\n");
}

void exec_SRAI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = (int32_t)cpu->regs[rs1(inst)] >> imm;
    print_op("srai\n");
}

void exec_ORI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] | imm;
    print_op("ori\n");
}

void exec_ANDI(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] & imm;
    print_op("andi\n");
}


void exec_ADD(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] =
        (u64) ((int64_t)cpu->regs[rs1(inst)] + (int64_t)cpu->regs[rs2(inst)]);
    print_op("add\n");
}

void exec_SUB(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] =
        (u64) ((int64_t)cpu->regs[rs1(inst)] - (int64_t)cpu->regs[rs2(inst)]);
    print_op("sub\n");
}

void exec_SLL(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] << (int64_t)cpu->regs[rs2(inst)];
    print_op("sll\n");
}

void exec_SLT(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < (int64_t) cpu->regs[rs2(inst)])?1:0;
    print_op("slt\n");
}

void exec_SLTU(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < cpu->regs[rs2(inst)])?1:0;
    print_op("slti\n");
}

void exec_XOR(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] ^ cpu->regs[rs2(inst)];
    print_op("xor\n");
}

void exec_SRL(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] >> cpu->regs[rs2(inst)];
    print_op("srl\n");
}

void exec_SRA(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int32_t)cpu->regs[rs1(inst)] >> 
        (int64_t) cpu->regs[rs2(inst)];
    print_op("sra\n");
}

void exec_OR(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] | cpu->regs[rs2(inst)];
    print_op("or\n");
}

void exec_AND(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] & cpu->regs[rs2(inst)];
    print_op("and\n");
}

void exec_FENCE(CPU* cpu, u32 inst) {
    print_op("fence\n");
}

void exec_ECALL(CPU* cpu, u32 inst) {}
void exec_EBREAK(CPU* cpu, u32 inst) {}

void exec_ECALLBREAK(CPU* cpu, u32 inst) {
    if (imm_I(inst) == 0x0)
        exec_ECALL(cpu, inst);
    if (imm_I(inst) == 0x1)
        exec_EBREAK(cpu, inst);
    print_op("ecallbreak\n");
}


void exec_ADDIW(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] + (int64_t) imm;
    print_op("addiw\n");
}

// TODO
void exec_SLLIW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] <<  shamt(inst));
    print_op("slliw\n");
}
void exec_SRLIW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] >>  shamt(inst));
    print_op("srliw\n");
}
void exec_SRAIW(CPU* cpu, u32 inst) {
    u64 imm = imm_I(inst);
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] >> (u64)(int64_t)(int32_t) imm);
    print_op("sraiw\n");
}
void exec_ADDW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] 
            + (int64_t) cpu->regs[rs2(inst)]);
    print_op("addw\n");
}
void exec_MULW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] 
            * (int64_t) cpu->regs[rs2(inst)]);
    print_op("mulw\n");
}
void exec_SUBW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] 
            - (int64_t) cpu->regs[rs2(inst)]);
    print_op("subw\n");
}
void exec_DIVW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] 
            / (int64_t) cpu->regs[rs2(inst)]);
    print_op("divw\n");
}
void exec_SLLW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] <<  cpu->regs[rs2(inst)]);
    print_op("sllw\n");
}
void exec_SRLW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] >>  cpu->regs[rs2(inst)]);
    print_op("srlw\n");
}
void exec_DIVUW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] / (int64_t) cpu->regs[rs2(inst)];
    print_op("divuw\n");
}
void exec_SRAW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] >>  (u64)(int64_t)(int32_t) cpu->regs[rs2(inst)]);
    print_op("sraw\n");
}
void exec_REMW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = (int64_t)(int32_t) (cpu->regs[rs1(inst)] 
            % (int64_t) cpu->regs[rs2(inst)]);
    print_op("remw\n");
}
void exec_REMUW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] % (int64_t) cpu->regs[rs2(inst)];
    print_op("remuw\n");
}


// ==================================================================== //
//                            CSR instructions
// ==================================================================== //

void exec_CSRRW(CPU* cpu, u32 inst) {
    cpu->regs[rd(inst)] = csr_read(cpu, csr(inst));
    csr_write(cpu, csr(inst), cpu->regs[rs1(inst)]);
    print_op("csrrw\n");
}
void exec_CSRRS(CPU* cpu, u32 inst) {
    csr_write(cpu, csr(inst), cpu->csr[csr(inst)] | cpu->regs[rs1(inst)]);
    print_op("csrrs\n");
}
void exec_CSRRC(CPU* cpu, u32 inst) {
    csr_write(cpu, csr(inst), cpu->csr[csr(inst)] & !(cpu->regs[rs1(inst)]) );
    print_op("csrrc\n");
}
void exec_CSRRWI(CPU* cpu, u32 inst) {
    csr_write(cpu, csr(inst), rs1(inst));
    print_op("csrrwi\n");
}
void exec_CSRRSI(CPU* cpu, u32 inst) {
    csr_write(cpu, csr(inst), cpu->csr[csr(inst)] | rs1(inst));
    print_op("csrrsi\n");
}
void exec_CSRRCI(CPU* cpu, u32 inst) {
    csr_write(cpu, csr(inst), cpu->csr[csr(inst)] & !rs1(inst));
    print_op("csrrci\n");
}

// AMO_W
void exec_LR_W(CPU* cpu, u32 inst) {}  
void exec_SC_W(CPU* cpu, u32 inst) {}  
void exec_AMOSWAP_W(CPU* cpu, u32 inst) {}  
void exec_AMOADD_W(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp + (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoadd.w\n");
} 
void exec_AMOXOR_W(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp ^ (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoxor.w\n");
} 
void exec_AMOAND_W(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp & (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoand.w\n");
} 
void exec_AMOOR_W(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp | (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoor.w\n");
} 
void exec_AMOMIN_W(CPU* cpu, u32 inst) {} 
void exec_AMOMAX_W(CPU* cpu, u32 inst) {} 
void exec_AMOMINU_W(CPU* cpu, u32 inst) {} 
void exec_AMOMAXU_W(CPU* cpu, u32 inst) {} 

// AMO_D TODO
void exec_LR_D(CPU* cpu, u32 inst) {}  
void exec_SC_D(CPU* cpu, u32 inst) {}  
void exec_AMOSWAP_D(CPU* cpu, u32 inst) {}  
void exec_AMOADD_D(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp + (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoadd.w\n");
} 
void exec_AMOXOR_D(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp ^ (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoxor.w\n");
} 
void exec_AMOAND_D(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp & (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoand.w\n");
} 
void exec_AMOOR_D(CPU* cpu, u32 inst) {
    u32 tmp = cpu_load(cpu, cpu->regs[rs1(inst)], 32);
    u32 res = tmp | (u32)cpu->regs[rs2(inst)];
    cpu->regs[rd(inst)] = tmp;
    cpu_store(cpu, cpu->regs[rs1(inst)], 32, res);
    print_op("amoor.w\n");
} 
void exec_AMOMIN_D(CPU* cpu, u32 inst) {} 
void exec_AMOMAX_D(CPU* cpu, u32 inst) {} 
void exec_AMOMINU_D(CPU* cpu, u32 inst) {} 
void exec_AMOMAXU_D(CPU* cpu, u32 inst) {} 


// ==================================================================== //
//                            Func API: CPU
// ==================================================================== //


 void cpu_init(CPU *cpu) {
    dram_init(&cpu->bus.dram);              // Init memory
    cpu->regs[0] = 0x00;                    // register x0 hardwired to 0
    cpu->regs[2] = DRAM_BASE + DRAM_SIZE;   // Set stack pointer
    cpu->pc      = DRAM_BASE;               // Set program counter to the base address
 }

u32 cpu_fetch(CPU *cpu) {
    u32 inst = bus_load(&(cpu->bus), cpu->pc, 32);
    return inst;
}

/**
 * @note 操作码告诉我们要对给定的地址和寄存器进行什么操作。
 * 具体要进行什么操作取决于 3 个值：
 * 操作码 opcode，funct3 和 funct7。
 * 根据指令映射，
 * 我们用以下 cpu_execute 函数对这 3 个部分进行译码。
 */
int cpu_execute(CPU *cpu, u32 inst) {
    int opcode = inst & 0x7f;           // opcode in bits 6..0
    int funct3 = (inst >> 12) & 0x7;    // funct3 in bits 14..12
    int funct7 = (inst >> 25) & 0x7f;   // funct7 in bits 31..25
    cpu->regs[0] = 0;                   // x0 hardwired to 0 at each cycle

    /*printf("%s\n%#.8lx -> Inst: %#.8x <OpCode: %#.2x, funct3:%#x, funct7:%#x> %s",*/
            /*ANSI_YELLOW, cpu->pc-4, inst, opcode, funct3, funct7, ANSI_RESET); // DEBUG*/
    printf(_yellow("\n%#.8lx -> "), cpu->pc-4); // DEBUG

    switch (opcode) {
        case LUI:   exec_LUI(cpu, inst); break;
        case AUIPC: exec_AUIPC(cpu, inst); break;

        case JAL:   exec_JAL(cpu, inst); break;
        case JALR:  exec_JALR(cpu, inst); break;

        case B_TYPE:
            switch (funct3) {
                case BEQ:   exec_BEQ(cpu, inst); break;
                case BNE:   exec_BNE(cpu, inst); break;
                case BLT:   exec_BLT(cpu, inst); break;
                case BGE:   exec_BGE(cpu, inst); break;
                case BLTU:  exec_BLTU(cpu, inst); break;
                case BGEU:  exec_BGEU(cpu, inst); break;
                default: ;
            } break;

        case LOAD:
            switch (funct3) {
                case LB  :  exec_LB(cpu, inst); break;  
                case LH  :  exec_LH(cpu, inst); break;  
                case LW  :  exec_LW(cpu, inst); break;  
                case LD  :  exec_LD(cpu, inst); break;  
                case LBU :  exec_LBU(cpu, inst); break; 
                case LHU :  exec_LHU(cpu, inst); break; 
                case LWU :  exec_LWU(cpu, inst); break; 
                default: ;
            } break;

        case S_TYPE:
            switch (funct3) {
                case SB  :  exec_SB(cpu, inst); break;  
                case SH  :  exec_SH(cpu, inst); break;  
                case SW  :  exec_SW(cpu, inst); break;  
                case SD  :  exec_SD(cpu, inst); break;  
                default: ;
            } break;

        case I_TYPE:  
            switch (funct3) {
                case ADDI:  exec_ADDI(cpu, inst); break;
                case SLLI:  exec_SLLI(cpu, inst); break;
                case SLTI:  exec_SLTI(cpu, inst); break;
                case SLTIU: exec_SLTIU(cpu, inst); break;
                case XORI:  exec_XORI(cpu, inst); break;
                case SRI:   
                    switch (funct7) {
                        case SRLI:  exec_SRLI(cpu, inst); break;
                        case SRAI:  exec_SRAI(cpu, inst); break;
                        default: ;
                    } break;
                case ORI:   exec_ORI(cpu, inst); break;
                case ANDI:  exec_ANDI(cpu, inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case R_TYPE:  
            switch (funct3) {
                case ADDSUB:
                    switch (funct7) {
                        case ADD: exec_ADD(cpu, inst);
                        case SUB: exec_ADD(cpu, inst);
                        default: ;
                    } break;
                case SLL:  exec_SLL(cpu, inst); break;
                case SLT:  exec_SLT(cpu, inst); break;
                case SLTU: exec_SLTU(cpu, inst); break;
                case XOR:  exec_XOR(cpu, inst); break;
                case SR:   
                    switch (funct7) {
                        case SRL:  exec_SRL(cpu, inst); break;
                        case SRA:  exec_SRA(cpu, inst); break;
                        default: ;
                    }
                case OR:   exec_OR(cpu, inst); break;
                case AND:  exec_AND(cpu, inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case FENCE: exec_FENCE(cpu, inst); break;

        case I_TYPE_64:
            switch (funct3) {
                case ADDIW: exec_ADDIW(cpu, inst); break;
                case SLLIW: exec_SLLIW(cpu, inst); break;
                case SRIW : 
                    switch (funct7) {
                        case SRLIW: exec_SRLIW(cpu, inst); break;
                        case SRAIW: exec_SRLIW(cpu, inst); break;
                    } break;
            } break;

        case R_TYPE_64:
            switch (funct3) {
                case ADDSUB:
                    switch (funct7) {
                        case ADDW:  exec_ADDW(cpu, inst); break;
                        case SUBW:  exec_SUBW(cpu, inst); break;
                        case MULW:  exec_MULW(cpu, inst); break;
                    } break;
                case DIVW:  exec_DIVW(cpu, inst); break;
                case SLLW:  exec_SLLW(cpu, inst); break;
                case SRW:
                    switch (funct7) {
                        case SRLW:  exec_SRLW(cpu, inst); break;
                        case SRAW:  exec_SRAW(cpu, inst); break;
                        case DIVUW: exec_DIVUW(cpu, inst); break;
                    } break;
                case REMW:  exec_REMW(cpu, inst); break;
                case REMUW: exec_REMUW(cpu, inst); break;
                default: ;
            } break;

        case CSR:
            switch (funct3) {
                case ECALLBREAK:    exec_ECALLBREAK(cpu, inst); break;
                case CSRRW  :  exec_CSRRW(cpu, inst); break;  
                case CSRRS  :  exec_CSRRS(cpu, inst); break;  
                case CSRRC  :  exec_CSRRC(cpu, inst); break;  
                case CSRRWI :  exec_CSRRWI(cpu, inst); break; 
                case CSRRSI :  exec_CSRRSI(cpu, inst); break; 
                case CSRRCI :  exec_CSRRCI(cpu, inst); break; 
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case AMO_W:
            switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
                case LR_W      :  exec_LR_W(cpu, inst); break;  
                case SC_W      :  exec_SC_W(cpu, inst); break;  
                case AMOSWAP_W :  exec_AMOSWAP_W(cpu, inst); break;  
                case AMOADD_W  :  exec_AMOADD_W(cpu, inst); break; 
                case AMOXOR_W  :  exec_AMOXOR_W(cpu, inst); break; 
                case AMOAND_W  :  exec_AMOAND_W(cpu, inst); break; 
                case AMOOR_W   :  exec_AMOOR_W(cpu, inst); break; 
                case AMOMIN_W  :  exec_AMOMIN_W(cpu, inst); break; 
                case AMOMAX_W  :  exec_AMOMAX_W(cpu, inst); break; 
                case AMOMINU_W :  exec_AMOMINU_W(cpu, inst); break; 
                case AMOMAXU_W :  exec_AMOMAXU_W(cpu, inst); break; 
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case 0x00:
            return 0;

        default:
            fprintf(stderr, 
                    "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct3:0x%x\n"
                    , opcode, funct3, funct7);
            return 0;
            /*exit(1);*/
    }
    return 1;
}

int cpu_step(CPU* cpu, int step) {
    if(step < 0) {
        while (1) {
            if (!cpu_step_one(cpu))
                return 0;
            cpu_dump_regs(cpu);
            if(cpu->pc==0)
                return 0;
        }
    } else {
        int i;
        for(i = 0; i < MIN(MAX_CPU_STEP, step); i++) {
            if (!cpu_step_one(cpu))
                return 0;
            cpu_dump_regs(cpu);
            if(cpu->pc==0)
                return 0;
        }
    }
    return 1;
}

/**
 * @note 三级流水线CPU
 * - 第 1 级流水线由函数`cpu_fetch()`处理。
 * - 第 2、3 级流水线由定义在`cpu.h`中的函数`cpu_execute()`一并处理。
 * - 程序计数器`pc`在每次循环后增加 4 个字节（32 位，因为每个 RISC-V 指令长度都为 32 位），
 * 以获取下一条指令。因此 CPU 执行循环可以被写为下面这样：
 */
int cpu_loop(CPU* cpu, char* filename) {
    // 1. 初始化：cpu, 寄存器 regs 和程序计数器 pc
    cpu_init(cpu);
    // 2. load_file(&cpu, argv[1]);
    load_elf(cpu, filename);
    // 3. 执行 cpu 循环
    cpu_step(cpu, -1);
    return 0;
}



void cpu_dump_regs(CPU* cpu) {
    char* abi[] = { // Application Binary Interface registers
        "zero", "ra",  "sp",  "gp",
          "tp", "t0",  "t1",  "t2",
          "s0", "s1",  "a0",  "a1",
          "a2", "a3",  "a4",  "a5",
          "a6", "a7",  "s2",  "s3",
          "s4", "s5",  "s6",  "s7",
          "s8", "s9", "s10", "s11",
          "t3", "t4",  "t5",  "t6",
    };

    /*for (int i=0; i<8; i++) {*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i],    i,    cpu->regs[i]);*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i+8],  i+8,  cpu->regs[i+8]);*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i+16], i+16, cpu->regs[i+16]);*/
        /*printf("%4s| x%02d: %#-8.2lx\n", abi[i+24], i+24, cpu->regs[i+24]);*/
    /*}*/

    for (int i=0; i<8; i++) {
        printf("   %4s: %#-13.2lx  ", abi[i],    cpu->regs[i]);
        printf("   %2s: %#-13.2lx  ", abi[i+8],  cpu->regs[i+8]);
        printf("   %2s: %#-13.2lx  ", abi[i+16], cpu->regs[i+16]);
        printf("   %3s: %#-13.2lx\n", abi[i+24], cpu->regs[i+24]);
    }
}