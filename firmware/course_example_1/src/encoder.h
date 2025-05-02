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

bool compare_pixels(unsigned char r1, unsigned char g1,unsigned char b1,unsigned char a1,unsigned char r2, unsigned char g2,unsigned char b2,unsigned char a2);
void initialize_previous_pixel(unsigned char r, unsigned char g,unsigned char b,unsigned char a);

//### chunks ###
unsigned char run_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
unsigned char get_hash(unsigned char r , unsigned char g,unsigned char b,unsigned char a);
unsigned char index_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
unsigned char diff_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
unsigned char luma_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
unsigned char rgb_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
void rgba_chunk(unsigned char r, unsigned char g,unsigned char b,unsigned char a);

void end_marker();
void output_header(unsigned  int width, unsigned  int height, unsigned char channels, unsigned char colorspace);

void output_chunk(unsigned char r,unsigned char g,unsigned char b,unsigned char a);
void output_chunk8(unsigned char input);
void output_chunk32(unsigned int input);

#endif
