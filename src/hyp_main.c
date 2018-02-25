/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <arch_helpers.h>
#include <debug.h>
#include <hyp.h>

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

void hyp_main(void)
{
	entry_point_info_t *ep = &entry_point_info;
	uint64_t mpidr;

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
	ep->spsr = 0x3c9;
	ep->pc = 0x10000000;

	cpu_data_init(mpidr);
	init_context(mpidr, ep);
	vm_init();
	hyp_init();
	hyp_enable();
	vm_boot();
}

void cpu_test(void)
{
	unsigned long cpu_id;

	pr_debug("-----Here is cpu bringup-----\n");
	pr_debug("midr_el1 = 0x%x\n", read_midr_el1());
	pr_debug("mpidr_el1 = 0x%x\n", read_mpidr_el1());

	cpu_id = (read_mpidr_el1() & 0xff00ffffff) + 1;

	arm_smccc_smc(0x84000000, 0, 0, 0, 0, 0, 0, 0, &cpu_res);
	pr_debug("res.a0 = 0x%x\n", cpu_res.a0);
	pr_debug("Start Next CPU %d\n", cpu_id);
	arm_smccc_smc(0xc4000003, cpu_id, (unsigned long)hyp_entrypoint, 0, 0, 0, 0, 0, &cpu_res);
	pr_debug("res a0 = %d, a1 = %d, a2 = %d, a3 = %d\n", cpu_res.a0, cpu_res.a1, cpu_res.a2, cpu_res.a3);
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
