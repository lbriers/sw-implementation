#include "math_ops.h"
#include "print.h"
#include "encoder.h"
#include "sensor.h"
#include <stdbool.h>

#define LED_BASEAxDDRESS 0x80000000

#define LED_REG0_ADDRESS (LED_BASEAxDDRESS + 0*4)

#define LED              (*(volatile unsigned int *) LED_REG0_ADDRESS)

unsigned int counter = 0;

void irq_handler(unsigned int cause) {
	//put interupts here	
}

unsigned int r;
unsigned int g;
unsigned int b;
unsigned int a;

void main(void) {
	//initialize pixels
	//initialize_previous_pixel(0x00, 0x00, 0x00, 0xFF);
	unsigned int width = SENSOR_get_width();	
	unsigned int height = SENSOR_get_height();	
	output_header(width, height,3,0);
	
	//while(SENSOR_SR & SENSOR_SR_FIRST == 0x00){
	//	SENSOR_fetch();
	//}
	//for(unsigned char i = 0; i < sw_mult(width,height)-1; i++){
	for(unsigned char i = 0; i < 64; i++){
		unsigned int data = SENSOR_fetch();
		r = (data >> 24);
		g = (data >> 16) & 0xFF;
		b = (data >> 8) & 0xFF;
		a = data & 0xFF;
		
		output_chunk(r, g, b, a);	
	}

	end_marker();

	while(1){
	}
}
