/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch_helpers.h>
#include <debug.h>
#include <platform_def.h>
#include <xlat_tables.h>


#define MAP_DEVICE      MAP_REGION_FLAT(DEVICE_BASE, DEVICE_SIZE, MT_DEVICE | MT_RW)
#define MAP_NS_DRAM     MAP_REGION_FLAT(DRAM_NS_BASE, DRAM_NS_SIZE, MT_DEVICE | MT_RW | MT_NS)

static const mmap_region_t rpi3_mmap[] = {
	MAP_DEVICE,
	MAP_NS_DRAM,
	{0}
};

void configure_mmu_el2(unsigned long ro_start, unsigned long ro_limit)
{
	mmap_add_region(ro_start, ro_start,
			ro_limit - ro_start, MT_MEMORY | MT_RO);
	mmap_add(rpi3_mmap);
	init_xlat_tables();

	enable_mmu_el2(0);
}

extern unsigned long __RO_START__;
extern unsigned long __RO_END__;

extern unsigned long __COHERENT_RAM_START__;
extern unsigned long __COHERENT_RAM_END__;

#define HYP_RO_BASE (unsigned long)(&__RO_START__)
#define HYP_RO_LIMIT (unsigned long)(&__RO_END__)

#define HYP_END (unsigned long)(&__HYP_END__)

void mm_init(void)
{
	configure_mmu_el2(HYP_RO_BASE, HYP_RO_LIMIT);
}
