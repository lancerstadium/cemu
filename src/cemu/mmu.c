/**
 * @file mmu.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */


// ==================================================================== //
//                                Include
// ==================================================================== //


#include "mmu.h"
#include "log.h"

// ==================================================================== //
//                            Func API: MMU
// ==================================================================== //


size_t mmu_get_offset(u64 base_memory_addr, u64 guest_physics_addr) {
    size_t offset = guest_physics_addr - DRAM_BASE;
    // log_info("Offset: %d (0x%.8x)", offset, offset);
    return offset;
}

u64 mmu_GPA_to_HVA(u64 base_memory_addr, u64 guest_physics_addr) {
    return base_memory_addr + guest_physics_addr - DRAM_BASE;
}

u64 mmu_HVA_to_GPA(u64 base_memory_addr, u64 host_virtual_addr) {
    return host_virtual_addr - base_memory_addr + DRAM_BASE;
}
