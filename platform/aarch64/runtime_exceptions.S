#include <arch.h>
#include <asm_macros.S>
#include <context.h>
#include <interrupt.h>
#include <platform_def.h>
#include <runtime_svc.h>

	.globl	runtime_exceptions
	.globl	el2_exit

	/* -----------------------------------------------------
	 * Handle HVC exceptions separately from other sync
	 * exceptions.
	 * -----------------------------------------------------
	 */
	.macro	handle_sync_exception
	/* Enable the SError interrupt */
	msr	daifclr, #DAIF_ABT_BIT
	msr	daifclr, #DAIF_DBG_BIT

	str	x30, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_LR]
	mrs	x30, esr_el2
	ubfx	x30, x30, #ESR_EC_SHIFT, #ESR_EC_LENGTH

	cmp	x30, #EC_AARCH32_HVC
	b.eq	hvc_handler32

	cmp	x30, #EC_AARCH64_HVC
	b.eq	hvc_handler64

	cmp	x30, #EC_AARCH32_SMC
	b.eq	smc_handler32

	cmp	x30, #EC_AARCH64_SMC
	b.eq	smc_handler64


	/* -----------------------------------------------------
	 * The following code handles any synchronous exception
	 * that is not an HVC.
	 * -----------------------------------------------------
	 */

	bl	report_unhandled_exception
	.endm


	/* -----------------------------------------------------
	 * This macro handles FIQ or IRQ interrupts
	 * -----------------------------------------------------
	 */
	.macro	handle_interrupt_exception label
	/* Enable the SError interrupt */
	msr	daifclr, #DAIF_ABT_BIT

	str	x30, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_LR]
	bl	save_gp_registers

	/* Switch to the runtime stack i.e. SP_EL0 */
	ldr	x2, [sp, #CTX_EL2STATE_OFFSET + CTX_RUNTIME_SP]
	mov	x20, sp
	msr	spsel, #0
	mov	sp, x2

	/* Jason adds for debug */
	mov     x0, x22
	bl	dbg_print
	mov     x0, #0x0060
	movk    x0, #0x4000, lsl #16
	mov	x1, x0
	ldr	x0, [x1]
	bl	dbg_print
	mrs	x0, cnthp_ctl_el2
	bl	dbg_print
	mov	x0, #0xd800 /* 19200 * 1000 * 5 = 96000000(0x5b8d800) */
	movk	x0, #0x05b8, lsl #16
	msr	cnthp_tval_el2, x0
	mrs	x0, cnthp_ctl_el2
	bl	dbg_print

#if 0
	/*
	 * Find out whether this is a valid interrupt type. If the
	 * interrupt controller reports a spurious interrupt then
	 * return to where we came from.
	bl	plat_ic_get_pending_interrupt_type
	cmp	x0, #INTR_TYPE_INVAL
	b.eq	interrupt_exit_\label
	 */

	/*
	 * Get the registered handler for this interrupt type. A
	 * NULL return value implies that an interrupt was generated
	 * for which there is no handler registered or the interrupt
	 * was routed incorrectly. This is a problem of the framework
	 * so report it as an error.
	 */
	bl	get_interrupt_type_handler
	cbz	x0, interrupt_error_\label
	mov	x21, x0

	mov	x0, #INTR_ID_UNAVAILABLE
#if IMF_READ_INTERRUPT_ID
	/*
	 * Read the id of the highest priority pending interrupt. If
	 * no interrupt is asserted then return to where we came from.
	 */
	mov	x19,  #INTR_ID_UNAVAILABLE
	bl	plat_ic_get_pending_interrupt_id
	cmp	x19, x0
	b.eq	interrupt_exit_\label
#endif
#endif /* 0 */

	/*
	 * Save the EL2 system registers needed to return from
	 * this exception.
	 */
	mrs	x3, spsr_el2
	mrs	x4, elr_el2
	stp	x3, x4, [x20, #CTX_EL2STATE_OFFSET + CTX_SPSR_EL2]

	/* Restore the reference to the 'handle' i.e. SP_EL2 */
	mov	x2, x20

	/*  x3 will point to a cookie (not used now) */
	mov	x3, xzr

#if 0
	/* Call the interrupt type handler */
	blr	x21
#endif /* 0 */

interrupt_exit_\label:
	/* Return from exception, possibly in a different security state */
	b	el2_exit

	/*
	 * This label signifies a problem with the interrupt management
	 * framework where it is not safe to go back to the instruction
	 * where the interrupt was generated.
	 */
interrupt_error_\label:
	bl	report_unhandled_interrupt
	.endm


	.macro save_x18_to_x29_sp_el0
	stp	x18, x19, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X18]
	stp	x20, x21, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X20]
	stp	x22, x23, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X22]
	stp	x24, x25, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X24]
	stp	x26, x27, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X26]
	stp	x28, x29, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X28]
	mrs	x18, sp_el0
	str	x18, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_SP_EL0]
	.endm

	.section	.vectors, "ax"; .align 11
	.align	7
runtime_exceptions:
	/* -----------------------------------------------------
	 * Current EL with _sp_el0 : 0x0 - 0x200
	 * -----------------------------------------------------
	 */
sync_exception_sp_el0:
	/* -----------------------------------------------------
	 * We don't expect any synchronous exceptions from EL2
	 * -----------------------------------------------------
	 */
	bl	report_unhandled_exception
	check_vector_size sync_exception_sp_el0

	.align	7
	/* -----------------------------------------------------
	 * EL2 code is non-reentrant. Any asynchronous exception
	 * is a serious error. Loop infinitely.
	 * -----------------------------------------------------
	 */
irq_sp_el0:
	bl	report_unhandled_interrupt
	check_vector_size irq_sp_el0

	.align	7
fiq_sp_el0:
	bl	report_unhandled_interrupt
	check_vector_size fiq_sp_el0

	.align	7
serror_sp_el0:
	bl	report_unhandled_exception
	check_vector_size serror_sp_el0

	/* -----------------------------------------------------
	 * Current EL with SPx: 0x200 - 0x400
	 * -----------------------------------------------------
	 */
	.align	7
sync_exception_sp_elx:
	/* -----------------------------------------------------
	 * This exception will trigger if anything went wrong
	 * during a previous exception entry or exit or while
	 * handling an earlier unexpected synchronous exception.
	 * There is a high probability that SP_EL2 is corrupted.
	 * -----------------------------------------------------
	 */
	bl	report_unhandled_exception
	check_vector_size sync_exception_sp_elx

	.align	7
irq_sp_elx:
	bl	report_unhandled_interrupt
	check_vector_size irq_sp_elx

	.align	7
fiq_sp_elx:
	bl	report_unhandled_interrupt
	check_vector_size fiq_sp_elx

	.align	7
serror_sp_elx:
	bl	report_unhandled_exception
	check_vector_size serror_sp_elx

	/* -----------------------------------------------------
	 * Lower EL using AArch64 : 0x400 - 0x600
	 * -----------------------------------------------------
	 */
	.align	7
sync_exception_aarch64:
	/* -----------------------------------------------------
	 * This exception vector will be the entry point for
	 * HVCs and traps that are unhandled at lower ELs most
	 * commonly. SP_EL2 should point to a valid cpu context
	 * where the general purpose and system register state
	 * can be saved.
	 * -----------------------------------------------------
	 */
	handle_sync_exception
	check_vector_size sync_exception_aarch64

	.align	7
	/* -----------------------------------------------------
	 * Asynchronous exceptions from lower ELs are not
	 * currently supported. Report their occurrence.
	 * -----------------------------------------------------
	 */
irq_aarch64:
	mov     x22, #0xa860
	movk    x22, #0x0000, lsl #16
	handle_interrupt_exception irq_aarch64
	check_vector_size irq_aarch64

	.align	7
fiq_aarch64:
	mov     x22, #0xa861
	movk    x22, #0x0000, lsl #16
	handle_interrupt_exception fiq_aarch64
	check_vector_size fiq_aarch64

	.align	7
serror_aarch64:
	bl	report_unhandled_exception
	check_vector_size serror_aarch64

	/* -----------------------------------------------------
	 * Lower EL using AArch32 : 0x600 - 0x800
	 * -----------------------------------------------------
	 */
	.align	7
sync_exception_aarch32:
	/* -----------------------------------------------------
	 * This exception vector will be the entry point for
	 * HVCs and traps that are unhandled at lower ELs most
	 * commonly. SP_EL2 should point to a valid cpu context
	 * where the general purpose and system register state
	 * can be saved.
	 * -----------------------------------------------------
	 */
	handle_sync_exception
	check_vector_size sync_exception_aarch32

	.align	7
	/* -----------------------------------------------------
	 * Asynchronous exceptions from lower ELs are not
	 * currently supported. Report their occurrence.
	 * -----------------------------------------------------
	 */
irq_aarch32:
	mov     x22, #0xa832
	movk    x22, #0x0000, lsl #16
	handle_interrupt_exception irq_aarch32
	check_vector_size irq_aarch32

	.align	7
fiq_aarch32:
	mov     x22, #0xa832
	movk    x22, #0x0000, lsl #16
	handle_interrupt_exception fiq_aarch32
	check_vector_size fiq_aarch32

	.align	7
serror_aarch32:
	bl	report_unhandled_exception
	check_vector_size serror_aarch32

	.align	7

	/* -----------------------------------------------------
	 * The following code handles hypervisor calls.
	 *
	 * Note that x30 has been explicitly saved and can be
	 * used here
	 * -----------------------------------------------------
	 */
hvc_handler32:
hvc_handler64:
	bl save_gp_registers

	/* -----------------------------------------------------
	 * Save the SPSR_EL2, ELR_EL2, & HCR_EL2 in case there
	 * is a world switch during HVC handling.
	 * -----------------------------------------------------
	 */
	mrs	x16, spsr_el2
	mrs	x17, elr_el2
	mrs	x18, hcr_el2
	stp	x16, x17, [sp, #CTX_EL2STATE_OFFSET + CTX_SPSR_EL2]
	str	x18, [sp, #CTX_EL2STATE_OFFSET + CTX_HCR_EL2]

	/* -----------------------------------------------------
	 * Restore the saved C runtime stack value which will
	 * become the new SP_EL0 i.e. EL2 runtime stack. It was
	 * saved in the 'cpu_context' structure prior to the last
	 * ERET from EL2.
	 * -----------------------------------------------------
	 */
	ldr	x12, [sp, #CTX_EL2STATE_OFFSET + CTX_RUNTIME_SP]

	/* Switch to SP_EL0 */
	msr	spsel, #0

	mov	sp, x12

	/* -----------------------------------------------------
	 * ------------                                      ---
	 * --- TODO ---      handle hvc here actually        ---
	 * ------------                                      ---
	 * -----------------------------------------------------
	 */
	mrs	x0, esr_el2
	bl	handled_exception_print

	/* -----------------------------------------------------
	 * This routine assumes that the SP_EL2 is pointing to
	 * a valid context structure from where the gp regs and
	 * other special registers can be retrieved.
	 *
	 * Keep it in the same section as hvc_handler as this
	 * function uses a fall-through to el2_exit
	 * -----------------------------------------------------
	 */
el2_exit: ; .type el2_exit, %function
	/* -----------------------------------------------------
	 * Save the current SP_EL0 i.e. the EL2 runtime stack
	 * which will be used for handling the next HVC. Then
	 * switch to SP_EL2
	 * -----------------------------------------------------
	 */
	mov	x17, sp
	msr	spsel, #1
	str	x17, [sp, #CTX_EL2STATE_OFFSET + CTX_RUNTIME_SP]

	/* -----------------------------------------------------
	 * Restore SPSR_EL2, ELR_EL2 and HCR_EL2 prior to ERET
	 * -----------------------------------------------------
	 */
	ldr	x18, [sp, #CTX_EL2STATE_OFFSET + CTX_HCR_EL2]
	ldp	x16, x17, [sp, #CTX_EL2STATE_OFFSET + CTX_SPSR_EL2]
	msr	hcr_el2, x18
	msr	spsr_el2, x16
	msr	elr_el2, x17

	mov	x0, x18
	bl	dbg_print
	mov	x0, x16
	bl	dbg_print
	mov	x0, x17
	bl	dbg_print

	/* Restore saved general purpose registers and return */
	b	restore_gp_registers_eret


	/* -----------------------------------------------------
	 * The following code handles secure monitor calls.
	 *
	 * Note that x30 has been explicitly saved and can be
	 * used here
	 * -----------------------------------------------------
	 */
func smc_handler
smc_handler32:
smc_handler64:
	smc	#0
	eret

	/* -----------------------------------------------------
	 * The following functions are used to saved and restore
	 * all the general pupose registers. Ideally we would
	 * only save and restore the callee saved registers when
	 * a world switch occurs but that type of implementation
	 * is more complex. So currently we will always save and
	 * restore these registers on entry and exit of EL2.
	 * These are not macros to ensure their invocation fits
	 * within the 32 instructions per exception vector.
	 * -----------------------------------------------------
	 */
func save_gp_registers
	stp	x0, x1, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X0]
	stp	x2, x3, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X2]
	stp	x4, x5, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X4]
	stp	x6, x7, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X6]
	stp	x8, x9, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X8]
	stp	x10, x11, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X10]
	stp	x12, x13, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X12]
	stp	x14, x15, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X14]
	stp	x16, x17, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X16]
	save_x18_to_x29_sp_el0
	ret

func restore_gp_registers_eret
	ldp	x0, x1, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X0]
	ldp	x2, x3, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X2]
	ldp	x4, x5, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X4]
	ldp	x6, x7, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X6]
	ldp	x8, x9, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X8]
	ldp	x10, x11, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X10]
	ldp	x12, x13, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X12]
	ldp	x14, x15, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X14]
	ldp	x18, x19, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X18]
	ldp	x20, x21, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X20]
	ldp	x22, x23, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X22]
	ldp	x24, x25, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X24]
	ldp	x26, x27, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X26]
	ldp	x28, x29, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X28]
	ldp	x30, x17, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_LR]
	msr	sp_el0, x17
	ldp	x16, x17, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X16]
	eret
