// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
#include "print.h"
#include "math_ops.h"

void print_chr(char ch)
{
	*((volatile unsigned int*)OUTPORT) = ch;
}

void print_str(const char *p)
{
	while (*p != 0)
		*((volatile unsigned int*)OUTPORT) = *(p++);
}

void print_hex(unsigned int val, int digits) {
	unsigned int index, max;
	int i; /* !! must be signed, because of the check 'i>=0' */
	char x;

	if(digits == 0)
		return;

	max = digits << 2;

	for (i = max-4; i >= 0; i -= 4) {
		index = val >> i;
		index = index & 0xF;
		x="0123456789ABCDEF"[index];
		*((volatile unsigned int*)OUTPORT) = x;
	}
	print_str("\n");
}

void print_dec(unsigned int val){
	unsigned int index;
	char x;
	int i = 0;
	unsigned int ansr[16];

	if(val == 0){
		*((volatile unsigned int*)OUTPORT) = '0';
		print_str("\n");
		return;
	}
	while(val > 0){
		index = umod(val, 10);
		val = udiv(val,10);
		index = index & 0xF;
		ansr[i] = index;
		i++;
	}
	
	while(i != 0){
		i--;
		x="0123456789"[ansr[i]];
		*((volatile unsigned int*)OUTPORT) = x;
	}
	print_str("\n");
}

void flash_leds(unsigned int val){	
	for(int i= 0; i< 6666000; i++){
		*((volatile unsigned int*)OUTPORT) = val;
	}
}
