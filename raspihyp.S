/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

	.globl	hyp_entrypoint

	.globl __getspin
	.globl __set_entries
	.globl __uboot_data

	/* -----------------------------------------------------
	 * bl31_entrypoint() is the cold boot entrypoint,
	 * executed only by the primary cpu.
	 * -----------------------------------------------------
	 */

hyp_entrypoint:
	b __skipspin
__getspin:
	b bl31_getspin
__set_entries:
	b bl31_uboot_set_entries
__uboot_data:
	.quad 0
	.quad 0
	.quad 0
__skipspin:
	/* ---------------------------------------------------------------
	 * Preceding bootloader has populated x0 with a pointer to a
	 * 'bl31_params' structure & x1 with a pointer to platform
	 * specific structure
	 * ---------------------------------------------------------------
	 */
#if !RESET_TO_BL31
	mov	x20, x0
	mov	x21, x1
#else
	/* ---------------------------------------------
	 * Set the CPU endianness before doing anything
	 * that might involve memory reads or writes.
	 * ---------------------------------------------
	 */
	mrs	x0, sctlr_el3
	bic	x0, x0, #SCTLR_EE_BIT
	msr	sctlr_el3, x0
	isb
#endif

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	hyp_main

	b	hyp_exit
