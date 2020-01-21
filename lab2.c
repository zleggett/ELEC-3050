/*====================================================*/
/* Zachary Leggett and Santhosh Thundena */
/* ELEC 3040/3050 - Lab 2 */
/* Decade counter controlled by two switches and outputted to LEDs */
/*====================================================*/

#include "STM32L1xx.h" /* Microcontroller information */

/* Define global variables */
unsigned char counter; /* value of counter */

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = switch 1 (sw1) */
/* PA2 = switch 2 (sw2) */
/* PC[3:0] = counter output */
/*---------------------------------------------------*/
void PinSetup () {
	/* Configure PA1 and PA2 as input pins to read switches */
	RCC->AHBENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	GPIOA->MODER &= ~(0x0000003C); /* General purpose input mode */
	/* Configure PC[3:0] as output pins to disply value of counter */
	RCC->AHBENR |= 0x04; /* Enable GPIOC clock (bit 2) */
	GPIOC->MODER &= ~(0x000000FF); /* Clear PC3-PC0 mode bits */
	GPIOC->MODER |= (0x00000055); /* General purpose output mode*/
}

/*------------------------------------------------*/
/* Count function - increment/decrement counter */
/* and output to LEDs */
/*------------------------------------------------*/
void count(unsigned char sw2) {
	if (sw2 == 1) { //count down
		if (counter == 0) {
			counter = 9;
		}
		else {
			counter = counter - 1;
		}	
	}
	else { //count up
		if (counter == 9) {
			counter = 0;
		}
		else {
			counter = counter + 1;
		}
	}
	GPIOC->BSRR |= (~counter & 0x0F) << 16; //reset bits
	GPIOC->BSRR |= (counter & 0x0F); //set bits
}

/*----------------------------------------------------------*/
/* Delay function - do nothing for about 0.5 second */
/*----------------------------------------------------------*/
void delay () {
	int i,j,n;
	for (i=0; i<10; i++) { //outer loop
		for (j=0; j<20000; j++) { //inner loop
			n = j; //dummy operation for single-step test
		} //do nothing
 }
}

/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/
int main(void) {
	unsigned char sw1; //state of switch 1: 1=start/0=stop
	unsigned char sw2; //state of switch 2: 1=count down/0=count up
	counter = 0; //initial counter value

	PinSetup(); //Configure GPIO pins

	/* Endless loop */
	while (1) { 
		sw1 = GPIOA->IDR & 0x00000002;
		sw2 = GPIOA->IDR & 0x00000004;
		if (sw1 == 1) {
			count(sw2);
		}
		delay();
	} /* repeat forever */
}
