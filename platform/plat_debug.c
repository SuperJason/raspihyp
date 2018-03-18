#include <arch_helpers.h>
#include <debug.h>

#define PRINT_SYSREG(reg_name)		\
	pr_debug(#reg_name"\t= 0x%x\n", read_ ##reg_name())
void print_sys_regs(void)
{
	pr_debug("Here is system regisers:\n");
	pr_debug("CurrentEl\t= 0x%lx\n", read_CurrentEl());

	PRINT_SYSREG(id_pfr1_el1);
	PRINT_SYSREG(id_aa64pfr0_el1);
	PRINT_SYSREG(daif);

	PRINT_SYSREG(spsr_el1);
	PRINT_SYSREG(spsr_el2);

	PRINT_SYSREG(spsr_abt);
	PRINT_SYSREG(spsr_und);
	PRINT_SYSREG(spsr_fiq);
	PRINT_SYSREG(spsr_irq);

	PRINT_SYSREG(elr_el1);
	PRINT_SYSREG(elr_el2);

	PRINT_SYSREG(midr_el1);
	PRINT_SYSREG(mpidr_el1);

	PRINT_SYSREG(hcr_el2);
	PRINT_SYSREG(hacr_el2);
	PRINT_SYSREG(hpfar_el2);
	PRINT_SYSREG(hstr_el2);

	PRINT_SYSREG(vbar_el1);
	PRINT_SYSREG(vbar_el2);

	PRINT_SYSREG(sctlr_el1);
	PRINT_SYSREG(sctlr_el2);

	PRINT_SYSREG(actlr_el1);
	PRINT_SYSREG(actlr_el2);

	PRINT_SYSREG(esr_el1);
	PRINT_SYSREG(esr_el2);

	PRINT_SYSREG(afsr0_el1);
	PRINT_SYSREG(afsr0_el2);
	PRINT_SYSREG(afsr1_el1);
	PRINT_SYSREG(afsr1_el2);

	PRINT_SYSREG(far_el1);
	PRINT_SYSREG(far_el2);

	PRINT_SYSREG(mair_el1);
	PRINT_SYSREG(mair_el2);

	PRINT_SYSREG(amair_el1);
	PRINT_SYSREG(amair_el2);

	/* if el3 implemented, these regs are not implemented
	 * Reset Vector Base Address Register
	PRINT_SYSREG(rvbar_el1);
	PRINT_SYSREG(rvbar_el2);

	 * Reset Management Register
	PRINT_SYSREG(rmr_el1);
	PRINT_SYSREG(rmr_el2);
	*/

	PRINT_SYSREG(tcr_el1);
	PRINT_SYSREG(tcr_el2);

	PRINT_SYSREG(ttbr0_el1);
	PRINT_SYSREG(ttbr0_el2);

	PRINT_SYSREG(ttbr1_el1);

	PRINT_SYSREG(cptr_el2);

	PRINT_SYSREG(cpacr_el1);

	PRINT_SYSREG(cntfrq_el0);
	PRINT_SYSREG(cnthctl_el2);
	PRINT_SYSREG(cnthp_ctl_el2);
	PRINT_SYSREG(cnthp_cval_el2);
	PRINT_SYSREG(cnthp_tval_el2);
	/* Hypervisor Virtal Timer is introduced in armv8.1
	PRINT_SYSREG(cnthv_ctl_el2);
	PRINT_SYSREG(cnthv_cval_el2);
	PRINT_SYSREG(cnthv_tval_el2);
	PRINT_SYSREG(cnthv_tval_el2);
	*/
	PRINT_SYSREG(cntkctl_el1);
	PRINT_SYSREG(cntp_ctl_el0);
	PRINT_SYSREG(cntp_cval_el0);
	PRINT_SYSREG(cntp_tval_el0);
	PRINT_SYSREG(cntpct_el0);
	/* Secure Timer 
	PRINT_SYSREG(cntps_ctl_el1);
	PRINT_SYSREG(cntps_tval_el1);
	PRINT_SYSREG(cntps_cval_el1);
	*/
	PRINT_SYSREG(cntv_ctl_el0);
	PRINT_SYSREG(cntv_cval_el0);
	PRINT_SYSREG(cntv_tval_el0);
	PRINT_SYSREG(cntvct_el0);
	PRINT_SYSREG(cntvoff_el2);

	PRINT_SYSREG(vpidr_el2);
	PRINT_SYSREG(vmpidr_el2);

	PRINT_SYSREG(isr_el1);

	/* PRINT_SYSREG(sp_el0); */
	PRINT_SYSREG(sp_el1);
	PRINT_SYSREG(spsel);

	/* GICv3 System Registers */
	/*
	PRINT_SYSREG(icc_sre_el1);
	PRINT_SYSREG(icc_sre_el2);
	PRINT_SYSREG(icc_pmr_el1);
	*/

	pr_debug("System regisers relevant to MMU:\n");

	/* DDI0487B_a_armv8_arm, Table D4-4 */
	PRINT_SYSREG(hcr_el2);
	PRINT_SYSREG(sctlr_el1);
	PRINT_SYSREG(sctlr_el2);
	/* sctlr_el3 */
	PRINT_SYSREG(tcr_el1);
	PRINT_SYSREG(tcr_el2);
	PRINT_SYSREG(vtcr_el2);
	/* tcr_el3 */
	PRINT_SYSREG(ttbr0_el1);
	PRINT_SYSREG(ttbr1_el1);
	PRINT_SYSREG(ttbr0_el2);
	PRINT_SYSREG(vttbr_el2);
	/* ttbr0_el3 */

	PRINT_SYSREG(id_aa64mmfr0_el1);
	PRINT_SYSREG(id_aa64mmfr1_el1);
	PRINT_SYSREG(hcr_el2);
	PRINT_SYSREG(mair_el1);
	PRINT_SYSREG(mair_el2);
	PRINT_SYSREG(amair_el1);
	PRINT_SYSREG(amair_el2);
	PRINT_SYSREG(contextidr_el1);
	PRINT_SYSREG(par_el1);
	PRINT_SYSREG(esr_el1);
	PRINT_SYSREG(esr_el2);
	PRINT_SYSREG(hpfar_el2);
	PRINT_SYSREG(far_el2);
}
