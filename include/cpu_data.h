#ifndef __CPU_DATA_H__
#define __CPU_DATA_H__

/* Offsets for the cpu_data structure */
#define CPU_DATA_CRASH_BUF_OFFSET	0x10
#define CPU_DATA_LOG2SIZE		7
/* need enough space in crash buffer to save 8 registers */
#define CPU_DATA_CRASH_BUF_SIZE		64
#define CPU_DATA_CPU_OPS_PTR		0x8

#include <arch_helpers.h>
#include <platform_def.h>
#include <cassert.h>
#include <stdint.h>
#include <assert.h>

/*******************************************************************************
 * Function & variable prototypes
 ******************************************************************************/

/*******************************************************************************
 * Cache of frequently used per-cpu data:
 *   Pointers to non-secure and secure security state contexts
 *   Address of the crash stack
 * It is aligned to the cache line boundary to allow efficient concurrent
 * manipulation of these pointers on different cpus
 *
 * TODO: Add other commonly used variables to this (tf_issues#90)
 *
 * The data structure and the _cpu_data accessors should not be used directly
 * by components that have per-cpu members. The member access macros should be
 * used for this.
 ******************************************************************************/
typedef struct cpu_data {
	void *cpu_context;
	uint64_t cpu_ops_ptr;
	uint64_t crash_buf[CPU_DATA_CRASH_BUF_SIZE >> 3];
} __aligned(CACHE_WRITEBACK_GRANULE) cpu_data_t;

/* verify assembler offsets match data structures */
CASSERT(CPU_DATA_CRASH_BUF_OFFSET == __builtin_offsetof
	(cpu_data_t, crash_buf),
	assert_cpu_data_crash_stack_offset_mismatch);

CASSERT((1 << CPU_DATA_LOG2SIZE) == sizeof(cpu_data_t),
	assert_cpu_data_log2size_mismatch);

CASSERT(CPU_DATA_CPU_OPS_PTR == __builtin_offsetof
		(cpu_data_t, cpu_ops_ptr),
		assert_cpu_data_cpu_ops_ptr_offset_mismatch);

cpu_data_t precpu_data[PLATFORM_CORE_COUNT];

/* Return the cpu_data structure for the current CPU. */
static inline struct cpu_data *_cpu_data(void)
{
	return (cpu_data_t *)read_tpidr_el2();
}


/**************************************************************************
 * APIs for initialising and accessing per-cpu data
 *************************************************************************/

void init_cpu_data_ptr(void);

static inline cpu_data_t *cpu_data_by_index(uint64_t ix)
{
	assert(ix >= PLATFORM_CORE_COUNT);
	return (cpu_data_t *)&precpu_data[ix];
}

static inline cpu_data_t *cpu_data_by_mpidr(uint64_t mpidr)
{
	uint64_t cluster, cpu;

	cpu = mpidr & MPIDR_CPU_MASK;
	cluster = mpidr & MPIDR_CLUSTER_MASK;

	cpu += (cluster >> 6);
	return cpu_data_by_index(cpu);
}

#define get_cpu_data(_m)		   _cpu_data()->_m
#define set_cpu_data(_m, _v)		   _cpu_data()->_m = _v

#define flush_cpu_data(_m)	   flush_dcache_range((uint64_t) 	  \
						      &(_cpu_data()->_m), \
						      sizeof(_cpu_data()->_m))
#define flush_cpu_data_by_index(_ix, _m)	\
				   flush_dcache_range((uint64_t)	  \
					 &(cpu_data_by_index(_ix)->_m),  \
					 sizeof(cpu_data_by_index(_ix)->_m))

#endif /* __CPU_DATA_H__ */
