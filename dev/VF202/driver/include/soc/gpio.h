/*
 * pinctrl/ingenic/pinctrl-ingenic.c
 *
 * Copyright 2015 Ingenic Semiconductor Co.,Ltd
 *
 * Author: cli <chen.li@inegnic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SV806_GPIO_H__
#define __SV806_GPIO_H__

#include <stdio.h>

#include <vbar/config.h>

#define GPIO_PA(n)  (0  + n)
#define GPIO_PB(n)  (16 + n)
#define GPIO_PC(n)  (44 + n)
#define GPIO_PD(n)  (67 + n)
#define GPIO_PE(n)  (73 + n)
#define GPIO_PF(n)  (88 + n)
#define GPIO_PG(n)  (98 + n)


enum gpio_function {
	GPIO_FUNC_0	= 0x00,  //0000, GPIO as function 0 / device 0
	GPIO_FUNC_1	= 0x01,  //0001, GPIO as function 1 / device 1
	GPIO_FUNC_2	= 0x02,  //0010, GPIO as function 2 / device 2
	GPIO_FUNC_3	= 0x03,  //0011, GPIO as function 3 / device 3
	GPIO_OUTPUT0	= 0x04,  //0100, GPIO output low  level
	GPIO_OUTPUT1	= 0x05,  //0101, GPIO output high level
	GPIO_INPUT	= 0x06,  //0110, GPIO as input
	GPIO_INT_LO 	= 0x08,  //1100, Low  Level trigger interrupt
	GPIO_INT_HI 	= 0x09,  //1101, High Level trigger interrupt
	GPIO_INT_FE 	= 0x0a,  //1110, Fall Edge trigger interrupt
	GPIO_INT_RE 	= 0x0b,  //1111, Rise Edge trigger interrupt
	GPIO_INT_RE_FE 	= 0x1a,  //Fall&Rise Edge trigger interrupt
};

#endif
