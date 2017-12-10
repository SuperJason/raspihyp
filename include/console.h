/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

int console_init(unsigned long base_addr,
		unsigned int uart_clk, unsigned int baud_rate);
int console_putc(int c);
int console_getc(void);

#endif /* __CONSOLE_H__ */

