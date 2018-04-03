/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch_helpers.h>
#include <debug.h>
#include <hyp.h>
#include <rpi3.h>

extern const char build_message[];
extern const char version_string[];

/**
 * struct arm_smccc_res - Result from SMC/HVC call
 * @a0-a3 result values from registers 0 to 3
 */
struct arm_smccc_res {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
};

/**
 * arm_smccc_smc() - make SMC calls
 * @a0-a7: arguments passed in registers 0 to 7
 * @res: result values from registers 0 to 3
 *
 * This function is used to make SMC calls following SMC Calling Convention.
 * The content of the supplied param are copied to registers 0 to 7 prior
 * to the SMC instruction. The return values are updated with the content
 * from register 0 to 3 on return from the SMC instruction.
 */
void arm_smccc_smc(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3, unsigned long a4,
			unsigned long a5, unsigned long a6, unsigned long a7,
			struct arm_smccc_res *res);

void cpu_test(void);
void mmu_test(void);
struct arm_smccc_res cpu_res;
void hyp_entrypoint(void);

void hyp_init(void);
void hyp_enable(void);

entry_point_info_t entry_point_info;
uint64_t one_run_el1;

void hyp_main(void)
{
	entry_point_info_t *ep = &entry_point_info;
	uint64_t mpidr;
	unsigned int *p;
	unsigned int i;
	uint64_t reg_val;

	dbg_print(0x10);
	dbg_print_sp();
	pr_notice("This is a hypervisor for Raspiberry Pi 3!\n");
	pr_notice("%s, %s\n", version_string, build_message);
	print_sys_regs();
	pr_notice("After print_sys_regs\n");

	mmu_test();

	cpu_test();

	mm_init();

	mpidr = read_mpidr();
	ep->spsr = 0x3c5;
	ep->pc = 0x00080000;

#define DEBUG_BOOT_KERNEL 1
#define DEBUG_BOOT_E1_HYP 1
#if DEBUG_BOOT_KERNEL
	ep->pc = 0x00080000;
#elif DEBUG_BOOT_E1_HYP
	ep->pc = 0x06000000;
#endif

	cpu_data_init(mpidr);
	init_context(mpidr, ep);
	vm_init();
	hyp_init();
	hyp_enable();
	vm_boot_prepare();

	p = (unsigned int*)0x00080000;
	pr_debug("HYP: Try to read addr: 0x%llx\n", (unsigned long long)p);
	pr_debug(" [0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x\n\t  [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x\n",
		p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);

	p = (unsigned int*)0x40000000;
	for (i = 0; i < 0x100 / 4; i++) {
		pr_debug(" 0x%x:0x%x\n", &p[i], p[i]);
	}

	p = (unsigned int*)BCM_LP_CORE0_TIMER_IRQCNTL;
	pr_debug("BCM_LP_CORE0_TIMER_IRQCNTL 0x%x:0x%x\n", p, *p);
	/* System timer generates a fiq.
	*p = 0xff; */
	/* System timer generates a irq. */
	*p = 0x0f;
	p = (unsigned int*)BCM_LP_CORE_TIMER_LS;
	pr_debug("BCM_LP_CORE_TIMER_LS 0x%x:0x%x\n", p, *p);
	p = (unsigned int*)BCM_LP_CORE_TIMER_MS;
	pr_debug("BCM_LP_CORE_TIMER_MS 0x%x:0x%x\n", p, *p);

	p = (unsigned int*)BCM_LP_CORE0_TIMER_IRQCNTL;
	pr_debug("BCM_LP_CORE0_TIMER_IRQCNTL 0x%x:0x%x\n", p, *p);

	p = (unsigned int*)BCM_LP_CORE_TIMER_LS;
	pr_debug("BCM_LP_CORE_TIMER_LS 0x%x:0x%x\n", p, *p);
	p = (unsigned int*)BCM_LP_CORE_TIMER_MS;
	pr_debug("BCM_LP_CORE_TIMER_MS 0x%x:0x%x\n", p, *p);

	pr_debug("----- EL2 study -----\n");
	reg_val = read_hcr();
	pr_debug("hcr: (0x%x)%d\n", reg_val, reg_val);
	reg_val |= HCR_RW_BIT;
#if HYP_IRQ_HANDLE_ENABLE
	reg_val |= HCR_AMO_BIT;
	reg_val |= HCR_IMO_BIT;
	reg_val |= HCR_FMO_BIT;
#endif
	write_hcr(reg_val);
	reg_val = read_hcr();
	pr_debug("hcr: (0x%x)%d\n", reg_val, reg_val);
	pr_debug("----- Interrupts study -----\n");
	reg_val = read_daif();
	pr_debug("daif: (0x%x)%d\n", reg_val, reg_val);
	enable_irq();
	reg_val = read_daif();
	pr_debug("daif: (0x%x)%d\n", reg_val, reg_val);
	enable_fiq();
	reg_val = read_daif();
	pr_debug("daif: (0x%x)%d\n", reg_val, reg_val);
	enable_serror();
	reg_val = read_daif();
	pr_debug("daif: (0x%x)%d\n", reg_val, reg_val);

	pr_debug("----- System time study -----\n");
#define PRI3_MS	19200
	reg_val = read_cntpct_el0();
	pr_debug("cntpct_el0: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	reg_val = read_cnthp_ctl_el2();
	pr_debug("cnthp_ctl_el2: (0x%x)%d\n", reg_val, reg_val);
	reg_val |= 1;
	write_cnthp_ctl_el2(reg_val);
	reg_val = read_cnthp_ctl_el2();
	pr_debug("cnthp_ctl_el2: (0x%x)%d\n", reg_val, reg_val);
	reg_val = read_cnthp_cval_el2();
	pr_debug("cnthp_cval_el2: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	reg_val = read_cnthp_tval_el2();
	pr_debug("cnthp_tval_el2: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	reg_val = read_cntpct_el0();
	pr_debug("cntpct_el0: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	/* Generate a timer frq/irq after 10 second
	reg_val = PRI3_MS * 1000 * 10; */
	/* Generate a timer fiq/irq before eret
	reg_val = PRI3_MS * 40; */
	reg_val = PRI3_MS * 1000 * 7;
	write_cnthp_tval_el2(reg_val);
	reg_val = read_cnthp_cval_el2();
	pr_debug("cnthp_cval_el2: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	reg_val = read_cnthp_tval_el2();
	pr_debug("cnthp_tval_el2: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);

	reg_val = read_cnthp_ctl_el2();
	pr_debug("cnthp_ctl_el2: (0x%x)%d\n", reg_val, reg_val);
	for (i = 0; i < 10; i++) {
		reg_val = read_cntpct_el0();
		pr_debug("cntpct_el0: (0x%x)%d, %d msec\n", reg_val, reg_val, reg_val / PRI3_MS);
	}
	reg_val = read_cnthp_ctl_el2();
	pr_debug("cnthp_ctl_el2: (0x%x)%d\n", reg_val, reg_val);

	/* EL1 physical timer */
	reg_val = PRI3_MS * 1000 * 5;
	write_cntp_tval_el0(reg_val);
	reg_val = 1;
	write_cntp_ctl_el0(reg_val);

	/* EL1 virtual timer */
	reg_val = PRI3_MS * 1000 * 3;
	write_cntv_tval_el0(reg_val);
	reg_val = 1;
	write_cntv_ctl_el0(reg_val);

	prepare_el2_exit();
	pr_debug("Exit from %s()\n", __func__);
	one_run_el1 = 1;
}

void cpu_test(void)
{
	/*
	unsigned long cpu_id;
	*/

	pr_debug("-----Here is cpu bringup-----\n");
	pr_debug("midr_el1 = 0x%x\n", read_midr_el1());
	pr_debug("mpidr_el1 = 0x%x\n", read_mpidr_el1());

	/*
	cpu_id = (read_mpidr_el1() & 0xff00ffffff) + 1;

	arm_smccc_smc(0x84000000, 0, 0, 0, 0, 0, 0, 0, &cpu_res);
	pr_debug("res.a0 = 0x%x\n", cpu_res.a0);
	pr_debug("Start Next CPU %d\n", cpu_id);
	arm_smccc_smc(0xc4000003, cpu_id, (unsigned long)hyp_entrypoint, 0, 0, 0, 0, 0, &cpu_res);
	pr_debug("res a0 = %d, a1 = %d, a2 = %d, a3 = %d\n", cpu_res.a0, cpu_res.a1, cpu_res.a2, cpu_res.a3);
	*/
}

void mmu_test(void)
{
	/*
	unsigned int *p;
	unsigned long long i;

	p = (unsigned int *)0x06000000;
	pr_debug("HYP: Try to read addr: 0x%llx\n", (unsigned long long)p);
	pr_debug(" [0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x\n\t  [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x\n",
		p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);

	p = (unsigned int *)0x08000000;
	pr_debug("HYP: Try to read addr: 0x%llx\n", (unsigned long long)p);
	pr_debug(" [0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x\n\t  [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x\n",
		p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);

	p = (unsigned int *)0x08400000;
	pr_debug("HYP: Try to read addr: 0x%llx\n", (unsigned long long)p);
	pr_debug(" [0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x\n\t  [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x\n",
		p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);

	for (i = 0; i < 0x40000000; i += 0x200000) {
		p = (unsigned int *)i;
		pr_debug("HYP: Try to read addr: 0x%llx -- *p:0x%x\n", (unsigned long long)p, *p);

	}
	*/
}

void hyp_init(void)
{
}

void hyp_enable(void)
{
}

uint64_t virq_cnt;
uint64_t pirq_cnt;
uint64_t hpirq_cnt;
void irq_handler(unsigned long flag)
{
	uint64_t reg_val;
	uint64_t elr_el1 = 0;
	uint64_t spsr_el1 = 0;
	int current_el;

	spsr_el1 = spsr_el1;
	elr_el1 = elr_el1;

	current_el = read_CurrentEl();
	current_el >>= 2;
#if 0
	pr_debug("HYP(%d): %s(): flag: 0x%x, hpirq_cnt: %d, pirq_cnt: %d, virq_cnt: %d\n",
			current_el, __func__, flag, hpirq_cnt, pirq_cnt, virq_cnt);
#endif
	if ((flag & BCM_LP_INT_SRC_CNTHPIRQ) && (2 == current_el)) {
		hpirq_cnt++;
		pr_debug("HYP(%d): %s(): BCM_LP_INT_SRC_CNTHPIRQ: hpirq_cnt: %d\n", current_el, __func__, hpirq_cnt);
		reg_val = PRI3_MS * 1000 * 7;
		write_cnthp_tval_el2(reg_val);

	}

	if (flag & BCM_LP_INT_SRC_CNTVIRQ) {
		virq_cnt++;
		pr_debug("HYP(%d): %s(): BCM_LP_INT_SRC_CNTHPIRQ: virq_cnt: %d\n", current_el, __func__, virq_cnt);
		reg_val = PRI3_MS * 1000 * 5;
		write_cntv_tval_el0(reg_val);

	}

	if (flag & BCM_LP_INT_SRC_CNTPNSIRQ) {
		pirq_cnt++;
		pr_debug("HYP(%d): %s(): BCM_LP_INT_SRC_CNTPNSIRQ: pirq_cnt: %d\n", current_el, __func__, pirq_cnt);
		reg_val = PRI3_MS * 1000 * 3;
		write_cntp_tval_el0(reg_val);

	}

	reg_val = read_hcr();
	reg_val &= ~HCR_VSE_BIT;
	reg_val &= ~HCR_VI_BIT;
	reg_val &= ~HCR_VF_BIT;
	write_hcr(reg_val);

	reg_val = read_spsr_el2();
	spsr_el1 = reg_val;
	reg_val |= 0xc0;
	write_spsr_el2(reg_val);

	reg_val = read_elr_el2();
	elr_el1 = reg_val;

	reg_val = read_vbar_el1();
	reg_val += 0x480;
	write_elr_el2(reg_val);

	write_spsr_el1(spsr_el1);
	write_elr_el1(elr_el1);

	one_run_el1 = 1;
}

void el1_main(void)
{
	int current_el = read_CurrentEl();
	current_el >>= 2;

	if(one_run_el1) {
		one_run_el1 = 0;
		pr_debug("HYP(%d): %s(): one_run\n", current_el, __func__);
	}
}

void kernel_dbg_print(unsigned long x0, unsigned long x1, unsigned long x2)
{
	int current_el = read_CurrentEl();
	current_el >>= 2;

	pr_debug("HYP(%d): %s(): x0: %x, x1: %x, x2: %x(%d)\n", current_el, __func__, x0, x1, x2, x2);
}
