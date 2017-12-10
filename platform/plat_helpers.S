/* 
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */
#include <asm_macros.S>

	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl  plat_report_hypmsg


	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : x0, x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	mov x0, #1
	ret

	/* ---------------------------------------------
	 * int plat_crash_console_putc(int c)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func _wait_for_xmitr
nr:	
	mov     x1, #0x5054
	movk    x1, #0x3f21, lsl #16
	ldr     w1, [x1]
	tbz     w1, #5, nr
	ret

func _console_putc
	mov	x2, x30 /* lr */
	bl	_wait_for_xmitr
	mov     x1, #0x5040
	movk    x1, #0x3f21, lsl #16
	str	w0, [x1]
	mov	w1, w0
	and     w1, w1, #0xff
	cmp     w1, #0xa
	b.ne	xi
	bl	_wait_for_xmitr
	mov     x1, #0x5040
	movk    x1, #0x3f21, lsl #16
	mov     w0, #0xd
	str	w0, [x1]
xi:	mov	x30, x2
	ret
	
func plat_crash_console_putc
	b	_console_putc

hyp_msg:
	.asciz "\n### Hypervisor Message from ASM ###\n"
	
func plat_report_hypmsg
	mov	x9, x30
	adr	x3, hyp_msg
1:
	ldrb	w0, [x3], #0x1
	cbz	x0, 2f
	bl	plat_crash_console_putc
	b	1b
2:	
	ret	x9
