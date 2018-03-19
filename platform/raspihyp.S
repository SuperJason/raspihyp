/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch.h>
#include <asm_macros.S>

	.globl hyp_entrypoint
	.globl __uboot_data
	.globl hyp_exit
	.globl arm_smccc_smc

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
	bl	dbg_print
	mov	x0, #0xabcd
	mov	x1, #0x1234
	add	x0, x1, x0, LSL #16
	bl	dbg_print
	mrs     x0, CurrentEL
	bl	dbg_print

	/* ---------------------------------------------
	 * Check EL
	 * ---------------------------------------------
	 */
	bl	el2_check

	/* ---------------------------------------------
	 * Show something at the beginning of EL2
	 * ---------------------------------------------
	 */
	bl	plat_report_hypmsg

	/* Debug Start */
#if 0
	mov	x16, #0x3c5
	msr	spsr_el2, x16
	mov	x0, x16
	bl	dbg_print

	/* HYP boot in EL1 & EL2 successfully */
	mov	x0, #0x0600
	/* Linux boot in EL1 & EL2 failed
	mov	x0, #0x0008 */
	/* Linux boot in EL1 & EL2 failed
	mov	x0, #0x1000 */
	mov	x1, #0x0000
	add	x16, x1, x0, LSL #16
	msr	elr_el2, x16
	mov	x0, x16
	bl	dbg_print

	mov	x0, #0x8000
	mov	x1, #0x0002
	add	x16, x1, x0, LSL #16
	msr	hcr_el2, x16
	mov	x0, x16
	bl	dbg_print

	eret
#endif
	/* Debug End */

	/* ---------------------------------------------
	 * Set the CPU endianness before doing anything
	 * that might involve memory reads or writes.
	 * ---------------------------------------------
	 */
	mrs	x0, sctlr_el2
	bic	x0, x0, #SCTLR_EE_BIT
	msr	sctlr_el2, x0
	isb

	/* ---------------------------------------------
	 * Initialise cpu_data early to enable crash
	 * reporting to have access to crash stack.
	 * Since crash reporting depends on cpu_data to
	 * report the unhandled exception, not
	 * doing so can lead to recursive exceptions due
	 * to a NULL TPIDR_EL2
	 * ---------------------------------------------
	 */
	bl	init_cpu_data_ptr

	/* ---------------------------------------------
	 * Set the exception vector.
	 * ---------------------------------------------
	 */
	adr     x1, runtime_exceptions
	msr     vbar_el2, x1
	isb

	/* ---------------------------------------------
	 * Enable the SError interrupt now that the
	 * exception vectors have been setup.
	 * ---------------------------------------------
	 */
	msr	daifclr, #DAIF_ABT_BIT

	/* ---------------------------------------------
	 * Zero out NOBITS sections. There are 2 of them:
	 *   - the .bss section;
	 *   - the coherent memory section.
	 * ---------------------------------------------
	 */
	ldr	x0, =__BSS_START__
	ldr	x1, =__BSS_SIZE__
	bl	zeromem16

#if USE_COHERENT_MEM
	ldr	x0, =__COHERENT_RAM_START__
	ldr	x1, =__COHERENT_RAM_UNALIGNED_SIZE__
	bl	zeromem16
#endif

	/* ---------------------------------------------
	 * Use SP_EL0 for the C runtime stack.
	 * ---------------------------------------------
	 */
	msr	spsel, #0

	/* --------------------------------------------
	 * Allocate a stack. So far only the primary
	 * cpu is running at the moment.
	 * --------------------------------------------
	 */
	mrs	x0, mpidr_el1
	bl	platform_set_stack

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	hyp_main

	/* Jason adds for debug */
	mov     x0, #0xaafe
	movk    x0, #0x0000, lsl #16
	bl	dbg_print

	b	el2_exit

	/* -----------------------------------------------------
	 * Make sure it's booting at EL2, otherwise passby.
	 * -----------------------------------------------------
	 */
el2_check:
	mrs     x0, CurrentEL
	cmp     x0, #ID_AA64PFR0_EL2_SHIFT
	b.ne	1f

	ret
1:
	mrs	x0, sctlr_el1
	bic	x0, x0, #SCTLR_EE_BIT
	msr	sctlr_el1, x0
	isb

	bl	el1_dbg_print

	/* hvc	#0xbcda */
el1_spin:
	b	el1_spin


	/*
	 * linux/arch/arm64/kernel/smccc-call.S
	 */
arm_smccc_smc:
	.cfi_startproc
	smc	#0
	ldr	x4, [sp]
	stp	x0, x1, [x4, #0]
	stp	x2, x3, [x4, #16]
	ret
        .cfi_endproc
