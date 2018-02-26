/*
#include <arch.h>
#include <context.h>
#include <asm_macros.S>
#include <cpu_data.h>
#include <platform_def.h>
*/

	.globl	report_unhandled_exception
	.globl	report_unhandled_interrupt
	.globl	el3_panic

report_unhandled_exception:
report_unhandled_interrupt:
	b	crash_panic

crash_panic:
	b	crash_panic
