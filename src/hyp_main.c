/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <debug.h>

void hyp_main(void)
{
	int a, b, c;

	dbg_print(0x10);
	dbg_print_sp();
	pr_notice("This is a hypervisor for Raspiberry Pi 3!\n");
	a = 0;
	b = 0;

	a++;
	b++;

	c = a + b;

	c++;

}
