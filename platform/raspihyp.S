/* 
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch.h>
#include <asm_macros.S>

	.globl	hyp_entrypoint
	.globl __uboot_data
	.globl hyp_exit

	/* -----------------------------------------------------
	 * hyp_entrypoint() is the cold boot entrypoint,
	 * executed only by the primary cpu.
	 * -----------------------------------------------------
	 */

hyp_entrypoint:
	b __skipspin
__uboot_data:
	.quad 0
	.quad 0
	.quad 0
__skipspin:
	bl	plat_report_hypmsg
	bl	el2_setup

	/* ---------------------------------------------
	 * Use SP_EL0 for the C runtime stack.
	 * ---------------------------------------------
	 */
	msr	spsel, #0

	/* Jason Debug */
	mov	x0, #1
	bl	dbg_print
	bl	dbg_print_sp

	mrs	x0, mpidr_el1
	bl	platform_set_stack

	/* Jason Debug */
	mov	x0, #2
	bl	dbg_print
	bl	dbg_print_sp

	/* Jump to main function. */
	bl	hyp_main

	b	hyp_exit

	/* -----------------------------------------------------
	 * Make sure we boot at EL2, otherwise passby.
	 * -----------------------------------------------------
	 */
el2_setup:
	mrs     x0, CurrentEL
	cmp     x0, #ID_AA64PFR0_EL2_SHIFT
	ret

hyp_exit:
	eret

