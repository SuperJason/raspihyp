#ifndef __XLAT_TABLES_H__
#define __XLAT_TABLES_H__

/*
 * Flags to override default values used to program system registers while       
 * enabling the MMU.                                                             
 */                                                                              
#define DISABLE_DCACHE          (1 << 0)

#ifndef __ASSEMBLY__                                                             
#include <stdint.h>

/* Helper macro to define entries for mmap_region_t. It creates
 * identity mappings for each region.
 */
#define MAP_REGION_FLAT(adr, sz, attr) MAP_REGION(adr, adr, sz, attr)

/* Helper macro to define entries for mmap_region_t. It allows to
 * re-map address mappings from 'pa' to 'va' for each region.
 */
#define MAP_REGION(pa, va, sz, attr) {(pa), (va), (sz), (attr)}

/*
 * Flags for building up memory mapping attributes.
 * These are organised so that a clear bit gives a more restrictive  mapping
 * that a set bit, that way a bitwise-and two sets of attributes will never give
 * an attribute which has greater access rights that any of the original
 * attributes.
 */
typedef enum  {
	MT_DEVICE       = 0 << 0,
	MT_MEMORY       = 1 << 0,

	MT_RO           = 0 << 1,
	MT_RW           = 1 << 1,

	MT_SECURE       = 0 << 2,
	MT_NS           = 1 << 2
} mmap_attr_t;

typedef struct mmap_region {
	unsigned long   base_pa;
	unsigned long   base_va;
	unsigned long   size;
	mmap_attr_t     attr;
} mmap_region_t;

void mmap_add_region(unsigned long base_pa, unsigned long base_va,
		                                unsigned long size, unsigned attr);
void mmap_add(const mmap_region_t *mm);

void init_xlat_tables(void);

void enable_mmu_el1(uint32_t flags);
void enable_mmu_el2(uint32_t flags);


#endif /*__ASSEMBLY__*/                                                          
#endif /* __XLAT_TABLES_H__ */
