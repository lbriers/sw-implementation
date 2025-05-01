#include "math_ops.h"

unsigned int umult(unsigned int a, unsigned int b){
	/*
	unsigned int ans = 0;
	unsigned int i = a;	
	for(i; i!=0; i--){
		ans += b;
	}
	*/

	int result = 0; // Initialize result
  	while (b > 0) {
    		// If b is odd, add 'a' to result
    		if (b & 1) {
      			result = result + a;
    		}

    		// Left shift 'a' by 1 (equivalent to a * 2)
    		a = a << 1;

    		// Right shift 'b' by 1 (equivalent to b / 2)
    		b = b >> 1;
  	}
  	return result;
	
	//return ans;
}

unsigned int udiv(unsigned int a, unsigned int b){
	unsigned int answer = 0;

	unsigned int i = 16;
	do{
		i--;
		if(b << i <= a){
			a -= b << i;
			answer += 1 << i;
		}
	}while(i!=0);
	return answer;
}

unsigned int umod(unsigned int a, unsigned int b){	
	a = a-umult(udiv(a,b),b);
	return a;
}

void matrix_mult(struct matrix_t * z, struct matrix_t * x, struct matrix_t * y) {
  	z->a00 = sw_mult(x->a00, y->a00) + sw_mult(x->a10, y->a01) + sw_mult(x->a20, y->a02);
  	z->a10 = sw_mult(x->a00, y->a10) + sw_mult(x->a10, y->a11) + sw_mult(x->a20, y->a12);
  	z->a20 = sw_mult(x->a00, y->a20) + sw_mult(x->a10, y->a21) + sw_mult(x->a20, y->a22);

  	z->a01 = sw_mult(x->a01, y->a00) + sw_mult(x->a11, y->a01) + sw_mult(x->a21, y->a02);
  	z->a11 = sw_mult(x->a01, y->a10) + sw_mult(x->a11, y->a11) + sw_mult(x->a21, y->a12);
  	z->a21 = sw_mult(x->a01, y->a20) + sw_mult(x->a11, y->a21) + sw_mult(x->a21, y->a22);

  	z->a02 = sw_mult(x->a02, y->a00) + sw_mult(x->a12, y->a01) + sw_mult(x->a22, y->a02);
  	z->a12 = sw_mult(x->a02, y->a10) + sw_mult(x->a12, y->a11) + sw_mult(x->a22, y->a12);
  	z->a22 = sw_mult(x->a02, y->a20) + sw_mult(x->a12, y->a21) + sw_mult(x->a22, y->a22);
}
