/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <stdio.h>
#include <debug.h>
#include <stdarg.h>
#include <stdint.h>

static void unsigned_num_print(unsigned long int unum, unsigned int radix)
{
	/* Just need enough space to store 64 bit decimal integer */
	unsigned char num_buf[20];
	int i = 0 , rem;

	do {
		rem = unum % radix;
		if (rem < 0xa)
			num_buf[i++] = '0' + rem;
		else
			num_buf[i++] = 'a' + (rem - 0xa);
	} while (unum /= radix);

	while (--i >= 0)
		putchar(num_buf[i]);
}

static void string_print(const char *str)
{
	while (*str)
		putchar(*str++);
}

void hyp_printf(const char *fmt, ...)
{
	va_list args;
	int bit64;
	int64_t num;
	uint64_t unum;
	char *str;

	va_start(args, fmt);
	while (*fmt) {
		bit64 = 0;

		if (*fmt == '%') {
			fmt++;
			/* Check the format specifier */
loop:
			switch (*fmt) {
			case 'i': /* Fall through to next one */
			case 'd':
				if (bit64)
					num = va_arg(args, int64_t);
				else
					num = va_arg(args, int32_t);

				if (num < 0) {
					putchar('-');
					unum = (unsigned long int)-num;
				} else
					unum = (unsigned long int)num;

				unsigned_num_print(unum, 10);
				break;
			case 's':
				str = va_arg(args, char *);
				string_print(str);
				break;
			case 'x':
				if (bit64)
					unum = va_arg(args, uint64_t);
				else
					unum = va_arg(args, uint32_t);

				unsigned_num_print(unum, 16);
				break;
			case 'l':
				bit64 = 1;
				fmt++;
				goto loop;
			case 'u':
				if (bit64)
					unum = va_arg(args, uint64_t);
				else
					unum = va_arg(args, uint32_t);

				unsigned_num_print(unum, 10);
				break;
			default:
				/* Exit on any other format specifier */
				goto exit;
			}
			fmt++;
			continue;
		}
		putchar(*fmt++);
	}
exit:
	va_end(args);
}
