#include "convert.h"
#include "math_ops.h"

unsigned int convert(unsigned int x){
	x = x-32;
	x = umult(x, 5);
	x = udiv(x, 9);
	return x;
}
