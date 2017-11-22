/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch.h>

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
	/* ---------------------------------------------
	 * Set the CPU endianness before doing anything
	 * that might involve memory reads or writes.
	 * ---------------------------------------------
	 */
	mrs	x0, sctlr_el3
	bic	x0, x0, #SCTLR_EE_BIT
	msr	sctlr_el3, x0
	isb

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	hyp_main

	b	hyp_exit

hyp_exit:
	eret

