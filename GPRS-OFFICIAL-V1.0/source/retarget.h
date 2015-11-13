
#include <stdio.h>
#include <rt_misc.h>
#include <LPC213x.H>                       /* LPC213x definitions  */

#pragma import(__use_no_semihosting_swi)
#define CR 			0x0D

int sendchar (int ch);
int fputc(int ch, FILE *f);
int ferror(FILE *f);
void _ttywrch(int ch);
void _sys_exit(int return_code);
