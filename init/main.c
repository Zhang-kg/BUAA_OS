/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <printf.h>
#include <pmap.h>
struct my_struct {
	int size;
	char c;
	int array[];
};
int main()
{
	printf("main.c:\tmain is start ...\n");
	printf("%b\n", -10);
 printf("%%\n");
 printf("%0-0-0----d\n",24);
 printf("csd%/%d\n",24);
 printf("%0-021.21d\n",25);
 printf("%ld\n",-12312);
 printf("%053.78ld\n",-0000123);
 printf("sda%\n");
 printf("%s\n","cdss");
 printf("%x%X\n",1231,32);
 printf("%023lb\n", 231);
 printf("%lb\n", 144);
 printf("%lO\n", 1231);
 printf("%lx\n", 213);
 printf("%0--031u\n", 123);
 printf("%c\n", '-');	

	mips_init();
	panic("main is over is error!");

	return 0;
}
