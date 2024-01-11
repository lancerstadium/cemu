/**
 * @file opcode.h
 * @author lancer (lancerstadium@163.com)
 * @brief 操作码头文件
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 * # 指令译码
 * - 从 dram 中读取的用于执行的指令是 32 位宽的。
 * 这 32 位包含所有要执行的操作对应的指令，
 * 比如操作码，用于读/写的源寄存器和目标寄存器，
 * 立即数值等等。
 * 
 * - 对于不同类型的指令的译码是不同的，指令类型有以下几种：
 * 1. R 类型：寄存器类型指令
 * 2. I 类型：立即数类型指令
 * 3. S 类型：存储类型指令
 * 4. B 类型：中断类型指令
 * 5. U 类型：寄存器类型指令
 * 6. J 类型：跳转类型指令
 * 
 * - 下表展示了不同类型指令的寄存器的映射关系：
 * ```
 *  RISC-V Instuctions Types:
 *  
 *  |31            25|24       20|19       15|14    12|11        7|6            0|   32-bit
 *  +----------------------------------------------------------------------------+
 *  |     funct7     |    rs2    |    rs1    | funct3 |    rd     |    opcode    |   R-type
 *  |----------------------------------------------------------------------------|
 *  |           imm[11:0]        |    rs1    | funct3 |    rd     |    opcode    |   I-type
 *  |----------------------------------------------------------------------------|
 *  |    imm[11:5]   |    rs2    |    rs1    | funct3 |  imm[4:0] |    opcode    |   S-type
 *  |----------------------------------------------------------------------------|
 *  |   imm[12|10:5] |    rs2    |    rs1    | funct3 |imm[4:1|11]|    opcode    |   B-type  
 *  |----------------------------------------------------------------------------|
 *  |                   imm[32:12]                    |    rd     |    opcode    |   U-type
 *  |----------------------------------------------------------------------------|
 *  |              imm[20|10:1|11|19:12]              |    rd     |    opcode    |   J-type
 *  |----------------------------------------------------------------------------+
 * 
 * ```
 * 
 * 1. `opcode`： 低 7 位（inst[6:0]）用于区别每个 CPU 操作。
 * 2. `rd`：一个 4 位值（inst[11:7]），给定目标寄存器的地址。
 * 3. `funct3`：一个 3 位值（inst[14:12]），
 *      每一组类似操作的操作码相同（如 ADD，SUB，DIV，MUL 等等）。
 *      每组可包括至多 8 个不同指令，对应 8 个不同的`funct3`值。
 * 4. `funct7`：一个 7 位值（inst[31:25]）。与`funct3`类似，
 *      `funct7`将一组相同的`funct3`指令划分为多个指令。
 *      例如，SR（右移）有两个指令：
 *      `SRA`（算数右移）与`SRL`（逻辑右移）对应不同的`funct7`。
 * 5. `rs1`：一个 4 位值（inst[19:15]），给定源寄存器 1 的地址。
 * 6. `rs2`：一个 4 位值（inst[24:20]），给定源寄存器 2 的地址。
 * 7. `imm`：表示立即数。
 * 8. `shamt`：shamt 位于`imm`的低位，存有移位指令的移位量。
 * 
 */

#ifndef OPCODE_H
#define OPCODE_H

#define LUI     0x37 
#define AUIPC   0x17 

#define JAL     0x6f 
#define JALR    0x67 

#define B_TYPE  0x63
    #define BEQ     0x0
    #define BNE     0x1
    #define BLT     0x4
    #define BGE     0x5
    #define BLTU    0x6
    #define BGEU    0x7

#define LOAD    0x03
    #define LB      0x0
    #define LH      0x1
    #define LW      0x2
    #define LD      0x3
    #define LBU     0x4
    #define LHU     0x5
    #define LWU     0x6

#define S_TYPE  0x23
    #define SB      0x0
    #define SH      0x1
    #define SW      0x2
    #define SD      0x3

#define I_TYPE  0x13                /** opcode：I-type 0010011 */
    #define ADDI    0x0             /** funct3：ADDI   000 */
    #define SLLI    0x1             /** funct3：SLLI   001 */
    #define SLTI    0x2             /** funct3：SLTI   010 */
    #define SLTIU   0x3             /** funct3：SLTIU  011 */
    #define XORI    0x4             /** funct3：XORI   100 */
    #define SRI     0x5             /** funct3：SRI    101 */
        #define SRLI    0x00        /** funct7：SRLI   0000000 */
        #define SRAI    0x20        /** funct7：SRAI   0100000 */
    #define ORI     0x6             /** funct3：ORI    110 */
    #define ANDI    0x7             /** funct3：ANDI   111 */

#define R_TYPE  0x33
    #define ADDSUB  0x0
        #define ADD     0x00
        #define SUB     0x20
    #define SLL     0x1
    #define SLT     0x2
    #define SLTU    0x3
    #define XOR     0x4
    #define SR      0x5
        #define SRL     0x00
        #define SRA     0x20
    #define OR      0x6
    #define AND     0x7

#define FENCE   0x0f

#define I_TYPE_64 0x1b
    #define ADDIW   0x0
    #define SLLIW   0x1
    #define SRIW    0x5
        #define SRLIW   0x00
        #define SRAIW   0x20

#define R_TYPE_64 0x3b
    #define ADDSUB   0x0
        #define ADDW    0x00
        #define MULW    0x01
        #define SUBW    0x20
    #define DIVW    0x4
    #define SLLW    0x1
    #define SRW     0x5
        #define SRLW   0x00
        #define DIVUW   0x01
        #define SRAW   0x20
    #define REMW    0x6
    #define REMUW   0x7

#define CSR 0x73
    #define ECALLBREAK    0x00     // contains both ECALL and EBREAK
    #define CSRRW   0x01
    #define CSRRS   0x02
    #define CSRRC   0x03
    #define CSRRWI  0x05
    #define CSRRSI  0x06
    #define CSRRCI  0x07

#define AMO_W 0x2f
    #define LR_W        0x02
    #define SC_W        0x03
    #define AMOSWAP_W   0x01
    #define AMOADD_W    0x00
    #define AMOXOR_W    0x04
    #define AMOAND_W    0x0c
    #define AMOOR_W     0x08
    #define AMOMIN_W    0x10
    #define AMOMAX_W    0x14
    #define AMOMINU_W   0x18
    #define AMOMAXU_W   0x1c



#endif OPCODE_H