/**
 * @file bus.c
 * @author lancer (lancerstadium@163.com)
 * @brief 总线功能实现
 * @version 0.1
 * @date 2024-01-08
 * @copyright Copyright (c) 2024
 * 
 */


#include "bus.h"
#include "log.h"

// ==================================================================== //
//                            Func API: BUS
// ==================================================================== //

u64 bus_load(BUS* bus, u64 addr, u64 size) {
    
    u64 data_addr = dram_load_data(&(bus->dram), mmu_get_offset(bus->dram.mem_addr, addr), size);
    log_info("Bus load data addr: (0x%.8x)", data_addr);
    return data_addr;
}

void bus_store(BUS* bus, u64 addr, u64 size, u64 value) {
    dram_write_data(&(bus->dram), mmu_get_offset(bus->dram.mem_addr, addr), size, value);
}