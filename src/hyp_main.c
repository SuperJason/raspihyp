/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <debug.h>
extern const char build_message[];
extern const char version_string[];

void mmu_test(void);

void hyp_main(void)
{
	int a, b, c;

	dbg_print(0x10);
	dbg_print_sp();
	pr_notice("This is a hypervisor for Raspiberry Pi 3!\n");
	pr_notice("%s, %s\n", version_string, build_message);
	print_sys_regs();
	pr_notice("After print_sys_regs\n");

	mmu_test();

	a = 0;
	b = 0;

	a++;
	b++;

	c = a + b;

	c++;

}

void mmu_test(void)
{
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
}
