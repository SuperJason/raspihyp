#include <asm_macros.S>
#include <cpu_data.h>

	.globl	init_cpu_data_ptr
	.globl	_cpu_data_by_mpidr
	.globl	_cpu_data_by_index

/* -----------------------------------------------------------------
 * void init_cpu_data_ptr(void)
 *
 * Initialise the TPIDR_EL2 register to refer to the cpu_data_t
 * for the calling CPU. This must be called before cm_get_cpu_data()
 *
 * This can be called without a valid stack.
 * clobbers: x0, x1, x9, x10
 * -----------------------------------------------------------------
 */
func init_cpu_data_ptr
	mov	x10, x30
	mrs	x0, mpidr_el1
	bl	_cpu_data_by_mpidr
	msr	tpidr_el2, x0
	ret	x10


/* -----------------------------------------------------------------
 * cpu_data_t *_cpu_data_by_mpidr(uint64_t mpidr)
 *
 * Return the cpu_data structure for the CPU with given MPIDR
 *
 * This can be called without a valid stack. It assumes that
 * platform_get_core_pos() does not clobber register x9.
 * clobbers: x0, x1, x9
 * -----------------------------------------------------------------
 */
func _cpu_data_by_mpidr
	mov	x9, x30
	bl	platform_get_core_pos
	mov	x30, x9
	b	_cpu_data_by_index


/* -----------------------------------------------------------------
 * cpu_data_t *_cpu_data_by_index(uint32_t cpu_index)
 *
 * Return the cpu_data structure for the CPU with given linear index
 *
 * This can be called without a valid stack.
 * clobbers: x0, x1
 * -----------------------------------------------------------------
 */
func _cpu_data_by_index
	adr	x1, percpu_data
	add	x0, x1, x0, LSL #CPU_DATA_LOG2SIZE
	ret
