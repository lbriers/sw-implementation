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
unsigned char chunk_index = 0;
unsigned  int chunk_output = 0;

void initialize_previous_pixel(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
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
	unsigned char output;

	output = (input & 0xFF000000) >> 24;
	output_chunk8(output);

	output = (input & 0x00FF0000) >> 16;
	output_chunk8(output);

	output = (input & 0x0000FF00) >> 8;
	output_chunk8(output);

	output = input & 0x000000FF;
	output_chunk8(output);
	
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

void output_header(unsigned  int width, unsigned  int height, unsigned char channels, unsigned char colorspace){
	//initialize_previous_pixel(0x00, 0x00, 0x00, 0xFF);
	output_chunk8('q');
	output_chunk8('o');
	output_chunk8('i');
	output_chunk8('f');

	output_chunk32(width);
	output_chunk32(height);

	output_chunk8(channels);
	output_chunk8(colorspace);


}

bool compare_pixels(unsigned char r1, unsigned char g1,unsigned char b1,unsigned char a1,unsigned char r2, unsigned char g2,unsigned char b2,unsigned char a2){
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
unsigned char RLE = 0;

unsigned char run_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
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
			unsigned char tag = 192;
			unsigned char chunk = tag + RLE-1;
			//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
			output_chunk8(chunk);

			RLE = 0;
		}
		return 0;
	}
}

/*============================================
 * index CHUNK
 * ==========================================*/
unsigned char RA[64][4];

unsigned char get_hash(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	return (sw_mult(3,r) + sw_mult(5,g) +sw_mult(7,b) + sw_mult(11,a)) % 64;
}

unsigned char index_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	unsigned  int hash = get_hash(r,g,b,a);
	if(compare_pixels(RA[hash][0],RA[hash][1],RA[hash][2],RA[hash][3],r,g,b,a)){
		unsigned char chunk = hash & 63;
		//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
		output_chunk8(chunk);
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
unsigned char diff_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	unsigned char dr = 2 + r - previous_pixel_r;
	unsigned char dg = 2 + g - previous_pixel_g;
	unsigned char db = 2 + b - previous_pixel_b;
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
	unsigned char tag = 64;
	unsigned char chunk = dr + dg + db + tag;
	////output value to file
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	output_chunk8(chunk);

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

unsigned char luma_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	unsigned char dg = 32 + g - previous_pixel_g;	
	if(dg > 63){
		return 0;
	}
	unsigned char dr_dg = 8 + (r - previous_pixel_r) - dg - 32;
	if(dr_dg > 15){
		return 0;
	}
	unsigned char db_dg = 8 + (b - previous_pixel_b) - dg - 32;
	if(db_dg > 15){
		return 0;
	}
	unsigned char chunk = 128;
	chunk = chunk + (dg & 63);
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	output_chunk8(chunk);
	chunk = ((dr_dg & 15) << 4 ) + (db_dg & 15);
	//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
	output_chunk8(chunk);

	previous_pixel_r = r;
	previous_pixel_g = g;
	previous_pixel_b = b;
	previous_pixel_a = a;
	return 1;
}

/*============================================
 * RGB CHUNK
 * ==========================================*/
unsigned char rgb_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	if(previous_pixel_a == a){
		unsigned  int chunk = 0xFE000000;
		chunk = chunk + (r << 16) + (g << 8) + b;
		//chunk = reverse_endian(chunk);
		//fwrite(&chunk, sizeof(unsigned  int), 1, fptr);
		output_chunk32(chunk);

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
void rgba_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a){
	unsigned  int chunk = (r << 24) + (g << 16) + (b << 8) + a;
	//chunk = reverse_endian(chunk);
	unsigned char tag = 0xFF;
	//fwrite(&tag, sizeof(unsigned char), 1, fptr);
	output_chunk8(tag);
	//fwrite(&chunk, sizeof(unsigned  int), 1, fptr);
	output_chunk32(chunk);

	//previous_pixel = *pixel;
	return;
}
/*============================================
 * END MARKER
 * ==========================================*/
void end_marker(){
	if(RLE >= 0){
		unsigned char tag = 192;
		unsigned char chunk = tag + RLE;
		//fwrite(&chunk, sizeof(unsigned char), 1, fptr);
		output_chunk8(chunk);
		RLE = 0;

	}
	unsigned  int chunk = 0;
	//fwrite(&chunk, sizeof(unsigned  int), 1, fptr);
	output_chunk32(chunk);
	chunk = 1;
	//chunk = reverse_endian(chunk);
	//fwrite(&chunk, sizeof(unsigned  int), 1, fptr);
	output_chunk32(chunk);
}

/*============================================
 * RUN CODE
 * ==========================================*/
void output_chunk(unsigned int r,unsigned int g,unsigned int b,unsigned int a){
	if(run_chunk(r,g,b,a) == 1){
		return;	
	}
	else if(index_chunk(r,g,b,a) == 1){
		return;
	}
	else if(diff_chunk(r,g,b,a) == 1){
		return;
	}
	else if(luma_chunk(r,g,b,a) == 1){
		return;
	}
	rgb_chunk(r,g,b,a);
	return;
}
