#include <stdbool.h>
#include "encoder.h"
#include "print.h"
#include "math_ops.h"

unsigned int previous_pixel_r;
unsigned int previous_pixel_g;
unsigned int previous_pixel_b;
unsigned int previous_pixel_a;

/*============================================
 * OUTPUTTING CHUNKS
 * ==========================================*/
unsigned int chunk_index = 0;
unsigned int chunk_output = 0;

void initialize_previous_pixel(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	previous_pixel_r = r;
	previous_pixel_g = g;
	previous_pixel_b = b;
	previous_pixel_a = a;
}

void output_chunk8(unsigned char input){
	chunk_output += input << ((3 - chunk_index)*8);
	if(chunk_index == 3){
		//output chunk
		*((volatile unsigned int*)OUTPORT) = chunk_output;
		chunk_index = 0;
		chunk_output = 0;
		return;
	}
	chunk_index++;
	return;
}

void output_chunk32(unsigned int input){
	unsigned int output;

	output = (input & 0xFF000000) >> 24;
	output_chunk8(output & 0xFF);

	output = (input & 0x00FF0000) >> 16;
	output_chunk8(output & 0xFF);

	output = (input & 0x0000FF00) >> 8;
	output_chunk8(output & 0xFF);

	output = input & 0x000000FF;
	output_chunk8(output & 0xFF);
	
	return;
}



//function which reverses the endiannes of a unsigned  int (replaces htonl)
unsigned  int reverse_endian(unsigned  int input){
	unsigned  int output = 0;
	output += (input & 0x000000FF) << 24;
	output += (input & 0x0000FF00) << 8;
	output += (input & 0x00FF0000) >> 8;
	output += (input & 0xFF000000) >> 24;
	return output;
}

void output_header(unsigned  int width, unsigned  int height, unsigned int channels, unsigned int colorspace){
	initialize_previous_pixel(0x00, 0x00, 0x00, 0xFF);
	print_chr('q');
	print_chr('o');
	print_chr('i');
	print_chr('f');

	//output_chunk32(width);
	print_chr((width >> 24) & 0xff);
	print_chr((width >> 16) & 0xff);
	print_chr((width >> 8) & 0xff);
	print_chr(width & 0xff);
	//output_chunk32(height);
	print_chr((height >> 24) & 0xff);
	print_chr((height >> 16) & 0xff);
	print_chr((height >> 8) & 0xff);
	print_chr(height & 0xff);

	print_chr(channels & 0xff);
	print_chr(colorspace & 0xff);


}

bool compare_pixels(unsigned int r1, unsigned int g1,unsigned int b1,unsigned int a1,unsigned int r2, unsigned int g2,unsigned int b2,unsigned int a2){
	if(r1 != r2){
		return 0;
	}
	if(g1 != g2){
		return 0;
	}
	if(b1 != b2){
		return 0;
	}if(a1 != a2){
		return 0;
	}
	return 1;
}

/*============================================
 * RUN CHUNK
 * ==========================================*/
unsigned int RLE = 0;

unsigned int run_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	if(compare_pixels(r,g,b,a, previous_pixel_r, previous_pixel_g, previous_pixel_b, previous_pixel_a) == 1 && RLE <= 63){
		RLE++;
		previous_pixel_r = r;
		previous_pixel_g = g;
		previous_pixel_b = b;
		previous_pixel_a = a;
		return 1;
	}
	else{
		if(RLE > 0){
			//unsigned int tag = 192;
			//unsigned int chunk = tag + RLE-1;
			unsigned int chunk = 192 + RLE-1;
			//print_chr(RLE & 0xFF);
			//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
			//output_chunk8(chunk & 0xFF);
			print_chr(chunk & 0xFF);
			RLE = 0;
		}
		return 0;
	}
}

/*============================================
 * index CHUNK
 * ==========================================*/
unsigned int RA[64][4];

unsigned int get_hash(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	return (sw_mult(3,r) + sw_mult(5,g) +sw_mult(7,b) + sw_mult(11,a)) % 64;
}

unsigned int index_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	unsigned int hash = get_hash(r,g,b,a);
	if(compare_pixels(RA[hash][0],RA[hash][1],RA[hash][2],RA[hash][3],r,g,b,a)){
		unsigned int chunk = hash & 63;
		//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
		print_chr(chunk & 0xff);
		previous_pixel_r = r;
		previous_pixel_g = g;
		previous_pixel_b = b;
		previous_pixel_a = a;
		return 1;
	}
	RA[hash][0] = r;
	RA[hash][1] = g;
	RA[hash][2] = b;
	RA[hash][3] = a;
	return 0;
}
/*============================================
 * DIFF CHUNK
 * ==========================================*/
unsigned int diff_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	unsigned int dr = (2 + r - previous_pixel_r) & 0xff;
	unsigned int dg = (2 + g - previous_pixel_g) & 0xff;
	unsigned int db = (2 + b - previous_pixel_b) & 0xff;
	//check if the differences are within bounds
	if(dr > 3){
		return 0;
	}
	if(dg > 3){
		return 0;
	}
	if(db > 3){
		return 0;
	}

	//output values
	////convert difference values to two bits
	dr = (dr & 3) << 4;
	dg = (dg & 3) << 2;
	db = db & 3;
	////add everything together for the one byte
	unsigned int tag = 64;
	unsigned int chunk = dr + dg + db + tag;
	////output value to file
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	print_chr(chunk & 0xFF);

	//housekeeping
	previous_pixel_r = r;
	previous_pixel_g = g;
	previous_pixel_b = b;
	previous_pixel_a = a;
	return 1;
}

/*============================================
 * LUMA CHUNK
 * ==========================================*/

unsigned int luma_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	//output_chunk32(0xcafebabe);
	//output_chunk8(r & 0xFF);
	//output_chunk8(g & 0xFF);
	//output_chunk8(b & 0xFF);
	//output_chunk8(a & 0xFF);

	unsigned int dg = (32 + g - previous_pixel_g) & 0xff;	
	if(dg > 63){
		return 0;
	}
	unsigned int dr_dg = (8 + (r - previous_pixel_r) - dg - 32) & 0xff;
	if(dr_dg > 15){
		return 0;
	}
	unsigned int db_dg = (8 + (b - previous_pixel_b) - dg - 32) & 0xff;
	if(db_dg > 15){
		return 0;
	}
	unsigned int chunk = 128;
	chunk = (chunk + (dg & 63));
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	print_chr(chunk & 0xff);
	chunk = ((dr_dg & 15) << 4 ) + (db_dg & 15);
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	print_chr(chunk & 0xff);

	previous_pixel_r = r;
	previous_pixel_g = g;
	previous_pixel_b = b;
	previous_pixel_a = a;
	return 1;
}

/*============================================
 * RGB CHUNK
 * ==========================================*/
unsigned int rgb_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	if(previous_pixel_a == a){
		//unsigned int chunk = 0xFE000000;
		//chunk = chunk + (r << 16) + (g << 8) + b;
		//output_chunk32(chunk);
		print_chr(0xFE);
		print_chr(r & 0xff);
		print_chr(g & 0xff);
		print_chr(b & 0xff);

		previous_pixel_r = r;
		previous_pixel_g = g;
		previous_pixel_b = b;
		previous_pixel_a = a;
		return 1;
	}
	return 0;
}

/*============================================
 * RGBA CHUNK
 * ==========================================*/
void rgba_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a){
	//unsigned  int chunk = (r << 24) + (g << 16) + (b << 8) + a;
	//unsigned int tag = 0xFF;
	//output_chunk8(tag);
	//output_chunk32(chunk);
	print_chr(0xFF);
	print_chr(r & 0xff);
	print_chr(g & 0xff);
	print_chr(b & 0xff);
	print_chr(a & 0xff);

	return;
}
/*============================================
 * END MARKER
 * ==========================================*/
void end_marker(){
	if(RLE >= 0){
		unsigned int tag = 192;
		unsigned int chunk = tag + RLE-1;
		//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
		print_chr(chunk & 0xff);
		RLE = 0;

	}
	print_chr(0x00);
	print_chr(0x00);
	print_chr(0x00);
	print_chr(0x00);

	print_chr(0x00);
	print_chr(0x00);
	print_chr(0x00);
	print_chr(0x01);
}

/*============================================
 * RUN CODE
 * ==========================================*/
void output_chunk(unsigned int r,unsigned int g,unsigned int b,unsigned int a){

	if(run_chunk(r & 0xFF,g & 0xFF,b & 0xFF,a & 0xFF) == 1){
		return;	
	}
	else if(index_chunk(r & 0xFF,g & 0xFF,b & 0xFF,a & 0xFF) == 1){
		return;
	}
	else if(diff_chunk(r & 0xFF,g & 0xFF,b & 0xFF,a & 0xFF) == 1){
		return;
	}
	else if(luma_chunk(r & 0xFF,g & 0xFF,b & 0xFF,a & 0xFF) == 1){
		return;
	}
	rgb_chunk(r & 0xFF,g & 0xFF,b & 0xFF,a & 0xFF);
	return;
}
