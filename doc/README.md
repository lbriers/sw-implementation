# README

## Report
The goal of this assignment was the implementation of interupts for both button inputs and TCNT timer inputs. In order to test this a blink script was implemented.

### Code

```C
unsigned int counter = 0;


void irq_handler(unsigned int cause) {

    	if (cause & 1) {
		//TODO: ACK the reset
		TCNT_CR = TCNT_CR | 0x10;
		TCNT_CR = TCNT_CR ^ 0x10;
		counter++;
    	}
	if (cause & 16) {
		counter++;
	}

	//LED = counter;
	if((counter & 0x1) == 0x1){
		if((counter & 0x7) == 0x7){	
			LED = 0xF;
		}else{
			LED = 0x1;
		}
	} else {
		LED = 0x0;
	}
}


void main(void) {
	//TCNT_CMP = 0x100;
	TCNT_CMP = 0x1312D00;
	TCNT_CR = TCNT_CR | 0x07;
	LED = counter;
    	unsigned int i=1, j;
	/*
    	while(1) {
        	for(i=0;i<8;i++) {
            		LED = i;
        	}
    	}
	*/
	while(1){
	}
}
```

We start by setting the CMP value, which determines the period of the blinking. Next the timer is started, after which only the interupt interacts with the code.

The interupt uses bitmasks to determine the type of interupt. Both button and timer interupts increment a _counter_ value. Every interupt the counter value is checked to determine if an LED should be ON at that time (if LSB = 1). And then is checked if it is every 4th period (LSBs = "111").


### interupt duration
Another goal of the assignment was to measure the duration of an interupt. The starting cycle was recorded by looking for the first IMEM-adress of the trap-vector from the objectdump on the PC. Then the amount of cycles was counted until the last IMEM-address of the trap vector appeared on the PC.

N = 84 cycles.
