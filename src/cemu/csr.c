/**
 * @file csr.c
 * @author lancer (lancerstadium@163.com)
 * @brief CSR 
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// ==================================================================== //
//                              Include
// ==================================================================== //

#include "csr.h"


// ==================================================================== //
//                            Func API: CSR
// ==================================================================== //

uint64_t csr_read(CPU* cpu, uint64_t csr) {
    return (uint64_t)(uint32_t)cpu->csr[csr];
}

void csr_write(CPU* cpu, uint64_t csr, uint64_t value) {
    cpu->csr[csr] = value;
}