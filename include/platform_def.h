#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#define UART0_BASE			0x3f215040

#define DEVICE_BASE			0x3f000000
#define DEVICE_SIZE			0x01000000

/* The size of DDR RAM is 1GB. */
#define DRAM_BASE			0x00000000
#define DRAM_SIZE			0x40000000

/*
 *   - Non-Secure DRAM (remaining DRAM starting at DRAM_BASE)
 * TEE-shared nonsec starts at 0x20000 ; with 4MB shared
 */
# define DRAM_NS_BASE			(0x08000000)
# define DRAM_NS_SIZE			(4 * 1024 * 1024)

# define DRAM_SEC_BASE			(DRAM_NS_BASE + DRAM_NS_SIZE)	   /* ATF load address */
# define DRAM_SEC_SIZE			(28 * 1024 * 1024)
# define OPTEE_BASE			(DRAM_SEC_BASE + 0x20000) /* set above ATF by 64k... */

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

#define BL31_BASE			(DRAM_NS_BASE + DRAM_NS_SIZE)	   /* BL31 load address */
#define BL31_LIMIT			(BL31_BASE + 0x40000)

#define HYP_BASE			(0x06000000)
#define HYP_LIMIT			(HYP_BASE + 0x80000) /* HYP 512K */

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/
/* Size of cacheable stacks */
#define PLATFORM_STACK_SIZE		0x800

#define PLATFORM_CLUSTER_COUNT          1
#define PLATFORM_CORE_COUNT             4

#define HYP_VCPU_COUNT	4

/*******************************************************************************
 * Declarations and constants to access the mailboxes safely. Each mailbox is
 * aligned on the biggest cache line size in the platform. This is known only
 * to the platform as it might have a combination of integrated and external
 * caches. Such alignment ensures that two maiboxes do not sit on the same cache
 * line at any cache level. They could belong to different cpus/clusters &
 * get written while being protected by different locks causing corruption of
 * a valid mailbox address.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT   6
#define CACHE_WRITEBACK_GRANULE (1 << CACHE_WRITEBACK_SHIFT)

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define ADDR_SPACE_SIZE                 (1ull << 32)
#define MAX_XLAT_TABLES                 4
#define MAX_MMAP_REGIONS                16

#endif /* __PLATFORM_DEF_H__ */
