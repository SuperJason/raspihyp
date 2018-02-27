/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch_helpers.h>
#include <debug.h>
#include <context.h>
#include <cpu_data.h>
#include <assert.h>
#include <platform_def.h>
#include <string.h>
#include <hyp.h>

cpu_context_t vcpu_context[HYP_VCPU_COUNT];

/*******************************************************************************
 * This function is used to program the context that's used for exception
 * return. This initializes the SP_EL2 to a pointer to a 'cpu_context'
 ******************************************************************************/
static inline void set_next_context(void *context)
{
#if DEBUG
	uint64_t sp_mode;

	/*
	 * Check that this function is called with SP_EL0 as the stack
	 * pointer
	 */
	__asm__ volatile("mrs	%0, SPSel\n"
			 : "=r" (sp_mode));

	assert(sp_mode == MODE_SP_EL0);
#endif

	__asm__ volatile("msr	spsel, #1\n"
			 "mov	sp, %0\n"
			 "msr	spsel, #0\n"
			 : : "r" (context));
}

/*******************************************************************************
 * Prepare the CPU system registers for entry into EL1
 ******************************************************************************/
void prepare_el2_exit(void)
{
	cpu_context_t *ctx = get_cpu_data(cpu_context);

	assert(ctx);

	/* TODO:
	 * ...
	 *
	el1_sysregs_context_restore(get_sysregs_ctx(ctx));
	 */

	set_next_context(ctx);
}

void cpu_data_init(uint64_t mpidr)
{
	uint64_t vcpu, cluster, cpu;
	cpu_data_t *cpu_data_p = cpu_data_by_mpidr(mpidr);

	cluster = mpidr & MPIDR_CLUSTER_MASK;
	cpu = (mpidr & MPIDR_CPU_MASK) + (cluster >> 6);
	/* TODO: How to get vcpu from mpidr */
	vcpu = cpu;

	assert(cpu <= HYP_VCPU_COUNT);
	cpu_data_p->cpu_context = &vcpu_context[vcpu];

	write_tpidr_el2((uint64_t)cpu_data_p);
}

void init_context(uint64_t mpidr, entry_point_info_t *ep)
{
	cpu_context_t *ctx;
	el2_state_t *state;
	gp_regs_t *gp_regs;
	uint32_t hcr_el2;
	unsigned long sctlr_elx;

	ctx = cpu_data_by_mpidr(mpidr)->cpu_context;
	assert(ctx);

	/* Clear any residual register values from the context */
	memset(ctx, 0, sizeof(*ctx));

	/* TODO */
	hcr_el2 = read_hcr();
	hcr_el2 &= ~(HCR_IMO_BIT);
	write_hcr(hcr_el2);

	sctlr_elx = SCTLR_EL1_RES1 | SCTLR_EE_BIT;
	write_ctx_reg(get_sysregs_ctx(ctx), CTX_SCTLR_EL1, sctlr_elx);

	/* Populate EL2 state so that we've the right context before doing ERET */
	state = get_el2state_ctx(ctx);
	write_ctx_reg(state, CTX_HCR_EL2, hcr_el2);
	write_ctx_reg(state, CTX_ELR_EL2, ep->pc);
	write_ctx_reg(state, CTX_SPSR_EL2, ep->spsr);
	/*
	 * Store the X0-X7 value from the entrypoint into the context
	 * Use memcpy as we are in control of the layout of the structures
	 */
	gp_regs = get_gpregs_ctx(ctx);
	memcpy(gp_regs, (void *)&ep->args, sizeof(aapcs64_params_t));
}
