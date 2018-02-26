/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/*******************************************************************************
 * Constants that allow assembler code to access members of and the 'gp_regs'
 * structure at their correct offsets.
 ******************************************************************************/
#define CTX_GPREGS_OFFSET	0x0
#define CTX_GPREG_X0		0x0
#define CTX_GPREG_X1		0x8
#define CTX_GPREG_X2		0x10
#define CTX_GPREG_X3		0x18
#define CTX_GPREG_X4		0x20
#define CTX_GPREG_X5		0x28
#define CTX_GPREG_X6		0x30
#define CTX_GPREG_X7		0x38
#define CTX_GPREG_X8		0x40
#define CTX_GPREG_X9		0x48
#define CTX_GPREG_X10		0x50
#define CTX_GPREG_X11		0x58
#define CTX_GPREG_X12		0x60
#define CTX_GPREG_X13		0x68
#define CTX_GPREG_X14		0x70
#define CTX_GPREG_X15		0x78
#define CTX_GPREG_X16		0x80
#define CTX_GPREG_X17		0x88
#define CTX_GPREG_X18		0x90
#define CTX_GPREG_X19		0x98
#define CTX_GPREG_X20		0xa0
#define CTX_GPREG_X21		0xa8
#define CTX_GPREG_X22		0xb0
#define CTX_GPREG_X23		0xb8
#define CTX_GPREG_X24		0xc0
#define CTX_GPREG_X25		0xc8
#define CTX_GPREG_X26		0xd0
#define CTX_GPREG_X27		0xd8
#define CTX_GPREG_X28		0xe0
#define CTX_GPREG_X29		0xe8
#define CTX_GPREG_LR		0xf0
#define CTX_GPREG_SP_EL0	0xf8
#define CTX_GPREGS_END		0x100

/*******************************************************************************
 * Constants that allow assembler code to access members of and the 'el2_state'
 * structure at their correct offsets. Note that some of the registers are only
 * 32-bits wide but are stored as 64-bit values for convenience
 ******************************************************************************/
#define CTX_EL2STATE_OFFSET	(CTX_GPREGS_OFFSET + CTX_GPREGS_END)
#define CTX_HCR_EL2		0x0
#define CTX_RUNTIME_SP		0x8
#define CTX_SPSR_EL2		0x10
#define CTX_ELR_EL2		0x18
#define CTX_EL2STATE_END	0x20

/*******************************************************************************
 * Constants that allow assembler code to access members of and the
 * 'el1_sys_regs' structure at their correct offsets. Note that some of the
 * registers are only 32-bits wide but are stored as 64-bit values for
 * convenience
 ******************************************************************************/
#define CTX_SYSREGS_OFFSET	(CTX_EL2STATE_OFFSET + CTX_EL2STATE_END)
#define CTX_SPSR_EL1		0x0
#define CTX_ELR_EL1		0x8
#define CTX_SPSR_ABT		0x10
#define CTX_SPSR_UND		0x18
#define CTX_SPSR_IRQ		0x20
#define CTX_SPSR_FIQ		0x28
#define CTX_SCTLR_EL1		0x30
#define CTX_ACTLR_EL1		0x38
#define CTX_CPACR_EL1		0x40
#define CTX_CSSELR_EL1		0x48
#define CTX_SP_EL1		0x50
#define CTX_ESR_EL1		0x58
#define CTX_TTBR0_EL1		0x60
#define CTX_TTBR1_EL1		0x68
#define CTX_MAIR_EL1		0x70
#define CTX_AMAIR_EL1		0x78
#define CTX_TCR_EL1		0x80
#define CTX_TPIDR_EL1		0x88
#define CTX_TPIDR_EL0		0x90
#define CTX_TPIDRRO_EL0		0x98
#define CTX_DACR32_EL2		0xa0
#define CTX_IFSR32_EL2		0xa8
#define CTX_PAR_EL1		0xb0
#define CTX_FAR_EL1		0xb8
#define CTX_AFSR0_EL1		0xc0
#define CTX_AFSR1_EL1		0xc8
#define CTX_CONTEXTIDR_EL1	0xd0
#define CTX_VBAR_EL1		0xd8
#define CTX_VBAR_EL2		0xe0
#define CTX_VPIDR_EL2		0xe8
#define CTX_VMPIDR_EL2		0xf0
#define CTX_VTCR_EL2		0xf8
#define CTX_VTTBR_EL2		0x100
#define CTX_FP_FPEXC32_EL2	0x108
#define CTX_SYSREGS_END		0x180

#ifndef __ASSEMBLY__

#include <cassert.h>
#include <platform_def.h>	/* for CACHE_WRITEBACK_GRANULE */
#include <stdint.h>

/*
 * Common constants to help define the 'cpu_context' structure and its
 * members below.
 */
#define DWORD_SHIFT		3
#define DEFINE_REG_STRUCT(name, num_regs)	\
	typedef struct name {			\
		uint64_t _regs[num_regs];	\
	}  __aligned(16) name##_t

/* Constants to determine the size of individual context structures */
#define CTX_GPREG_ALL		(CTX_GPREGS_END >> DWORD_SHIFT)
#define CTX_SYSREG_ALL		(CTX_SYSREGS_END >> DWORD_SHIFT)
#define CTX_EL2STATE_ALL	(CTX_EL2STATE_END >> DWORD_SHIFT)

/*
 * AArch64 general purpose register context structure. Usually x0-x18,
 * lr are saved as the compiler is expected to preserve the remaining
 * callee saved registers if used by the C runtime and the assembler
 * does not touch the remaining. But in case of world switch during
 * exception handling, we need to save the callee registers too.
 */
DEFINE_REG_STRUCT(gp_regs, CTX_GPREG_ALL);

/*
 * AArch64 EL1 system register context structure for preserving the
 * architectural state during switches from one security state to
 * another in EL1.
 */
DEFINE_REG_STRUCT(el1_sys_regs, CTX_SYSREG_ALL);

/*
 * Miscellaneous registers used by EL2 firmware to maintain its state
 * across exception entries and exits
 */
DEFINE_REG_STRUCT(el2_state, CTX_EL2STATE_ALL);

/*
 * Macros to access members of any of the above structures using their
 * offsets
 */
#define read_ctx_reg(ctx, offset)	((ctx)->_regs[offset >> DWORD_SHIFT])
#define write_ctx_reg(ctx, offset, val)	(((ctx)->_regs[offset >> DWORD_SHIFT]) \
					 = val)

/*
 * Top-level context structure which is used by EL2 firmware to
 * preserve the state of a core at EL1 in one of the two security
 * states and save enough EL2 meta data to be able to return to that
 * EL and security state. The context management library will be used
 * to ensure that SP_EL2 always points to an instance of this
 * structure at exception entry and exit. Each instance will
 * correspond to either the secure or the non-secure state.
 */
typedef struct cpu_context {
	gp_regs_t gpregs_ctx;
	el2_state_t el2state_ctx;
	el1_sys_regs_t sysregs_ctx;
} cpu_context_t;

/* Macros to access members of the 'cpu_context_t' structure */
#define get_el2state_ctx(h)	(&((cpu_context_t *) h)->el2state_ctx)
#define get_sysregs_ctx(h)	(&((cpu_context_t *) h)->sysregs_ctx)
#define get_gpregs_ctx(h)	(&((cpu_context_t *) h)->gpregs_ctx)

/*
 * Compile time assertions related to the 'cpu_context' structure to
 * ensure that the assembler and the compiler view of the offsets of
 * the structure members is the same.
 */
CASSERT(CTX_GPREGS_OFFSET == __builtin_offsetof(cpu_context_t, gpregs_ctx), \
	assert_core_context_gp_offset_mismatch);
CASSERT(CTX_SYSREGS_OFFSET == __builtin_offsetof(cpu_context_t, sysregs_ctx), \
	assert_core_context_sys_offset_mismatch);
CASSERT(CTX_EL2STATE_OFFSET == __builtin_offsetof(cpu_context_t, el2state_ctx), \
	assert_core_context_el2state_offset_mismatch);

/*
 * Helper macro to set the general purpose registers that correspond to
 * parameters in an aapcs_64 call i.e. x0-x7
 */
#define set_aapcs_args0(ctx, x0)				do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X0, x0);	\
	} while (0);
#define set_aapcs_args1(ctx, x0, x1)				do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X1, x1);	\
		set_aapcs_args0(ctx, x0);				\
	} while (0);
#define set_aapcs_args2(ctx, x0, x1, x2)			do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X2, x2);	\
		set_aapcs_args1(ctx, x0, x1);				\
	} while (0);
#define set_aapcs_args3(ctx, x0, x1, x2, x3)			do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X3, x3);	\
		set_aapcs_args2(ctx, x0, x1, x2);			\
	} while (0);
#define set_aapcs_args4(ctx, x0, x1, x2, x3, x4)		do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X4, x4);	\
		set_aapcs_args3(ctx, x0, x1, x2, x3);			\
	} while (0);
#define set_aapcs_args5(ctx, x0, x1, x2, x3, x4, x5)		do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X5, x5);	\
		set_aapcs_args4(ctx, x0, x1, x2, x3, x4);		\
	} while (0);
#define set_aapcs_args6(ctx, x0, x1, x2, x3, x4, x5, x6)	do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X6, x6);	\
		set_aapcs_args5(ctx, x0, x1, x2, x3, x4, x5);		\
	} while (0);
#define set_aapcs_args7(ctx, x0, x1, x2, x3, x4, x5, x6, x7)	do {	\
		write_ctx_reg(get_gpregs_ctx(ctx), CTX_GPREG_X7, x7);	\
		set_aapcs_args6(ctx, x0, x1, x2, x3, x4, x5, x6);	\
	} while (0);

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
void el1_sysregs_context_save(el1_sys_regs_t *regs);
void el1_sysregs_context_restore(el1_sys_regs_t *regs);

#undef CTX_SYSREG_ALL
#undef CTX_GPREG_ALL
#undef CTX_EL2STATE_ALL

#endif /* __ASSEMBLY__ */

#endif /* __CONTEXT_H__ */
