#ifndef MATH_OPS_H
#define MATH_OPS_H

unsigned int udiv(unsigned int a, unsigned int b);
unsigned int umod(unsigned int a, unsigned int b);
unsigned int umult(unsigned int a, unsigned int b);
//sw mult in assembly
extern unsigned int sw_mult(unsigned int x, unsigned int y);
//extern unsigned int sw_mult(uint8_t x, uint8_t y);

//matrix multipliation
struct matrix_t{
	unsigned int a00;
	unsigned int a01;
	unsigned int a02;
	unsigned int a10;
	unsigned int a11;
	unsigned int a12;
	unsigned int a20;
	unsigned int a21;
	unsigned int a22;
};

void matrix_mult(struct matrix_t * z, struct matrix_t * x, struct matrix_t * y);

#endif
