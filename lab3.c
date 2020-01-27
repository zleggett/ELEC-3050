/*====================================================*/
/* Zachary Leggett and Santhosh Thundena */
/* ELEC 3040/3050 - Lab 3 */
/* Two decades counter controlled by two switches and outputted to LEDs */
/* Counters are set to count in opposite directions. */
/*====================================================*/

#include "STM32L1xx.h" /* Microcontroller information */

/* Define global variables */
unsigned char counter1; /* value of first counter */
unsigned char counter2; /* value of second counter */

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = swith 1 (sw1) */
/* PA2 = swith 2 (sw2) */
/* PC[3:0] = counter1 output */
/* PC[7:0] = counter2 output */
/*---------------------------------------------------*/
void PinSetup () {
	/* Configure PA1 and PA2 as input pins to read switches */
	RCC->AHBENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	GPIOA->MODER &= ~(0x0000003C); /* General purpose input mode */
	/* Configure PC[7:0] as output pins to disply value of counters */
	RCC->AHBENR |= 0x04; /* Enable GPIOC clock (bit 2) */
	GPIOC->MODER &= ~(0x0000FFFF); /* Clear PC7-PC0 mode bits */
	GPIOC->MODER |= (0x00005555); /* General purpose output mode*/
}

/*------------------------------------------------*/
/* Count function - increment/decrement counter */
/* and output to LEDs */
/*------------------------------------------------*/
void count(unsigned char sw2) {
	if (sw2 != 0) { //count down
		if (counter1 == 0) {
			counter1 = 9;
		}
		else {
			counter1 = counter1 - 1;
		}
		
		if (counter2 == 9) {
			counter2 = 0;
		}
		else {
			counter2 = counter2 + 1;
		}
	}
	else { //count up
		if (counter1 == 9) {
			counter1 = 0;
		}
		else {
			counter1 = counter1 + 1;
		}
		
		if (counter2 == 0) {
			counter2 = 9;
		}
		else {
			counter2 = counter2 - 1;
		}
		
	}
	GPIOC->BSRR |= (~counter1 & 0x0F) << 16; //reset bits
	GPIOC->BSRR |= (counter1 & 0x0F); //set bits
	
	GPIOC->BSRR |= (~counter2 & 0xF0) << 16; //reset bits
	GPIOC->BSRR |= (counter2 & 0xF0); //set bits
	
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
	counter1 = 0; //initial counter value
	counter2 = 0;

	PinSetup(); //Configure GPIO pins

	/* Endless loop */
	while (1) { 
		sw1 = GPIOA->IDR & 0x00000002; //PA1
		sw2 = GPIOA->IDR & 0x00000004; //PA2
		if (sw1 != 0) {
			count(sw2);
		}
		delay();
	} /* repeat forever */
}
