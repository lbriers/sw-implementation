#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>


//#define CCD_BASEADDRESS        	0x82000000
#define STORE_BASEADDRESS	0x83000000

struct rgba_pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

bool compare_pixels(unsigned int r1, unsigned int g1,unsigned int b1,unsigned int a1,unsigned int r2, unsigned int g2,unsigned int b2,unsigned int a2);
void initialize_previous_pixel(unsigned int r, unsigned int g,unsigned int b,unsigned int a);

//### chunks ###
unsigned int run_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);
unsigned int get_hash(unsigned int r , unsigned int g,unsigned int b,unsigned int a);
unsigned int index_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);
unsigned int diff_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);
unsigned int luma_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);
unsigned int rgb_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);
void rgba_chunk(unsigned int r, unsigned int g,unsigned int b,unsigned int a);

void end_marker();
void output_header(unsigned  int width, unsigned  int height, unsigned int channels, unsigned int colorspace);

void output_chunk(unsigned int r,unsigned int g,unsigned int b,unsigned int a);
void output_chunk8(unsigned char input);
void output_chunk32(unsigned int input);

#endif
