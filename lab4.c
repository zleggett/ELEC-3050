/*====================================================*/
/* Zachary Leggett and Santhosh Thundena */
/* ELEC 3040/3050 - Lab 4 */
/* Two decades counter controlled by two switches and outputted to LEDs */
/* Counters are set to count in opposite directions. */
/*====================================================*/

#include "STM32L1xx.h" /* Microcontroller information */

/* Define global variables */
unsigned char c1; /* value of first counter */
unsigned char c2; /* value of second counter */
unsigned char c2_direction;
unsigned char LED8;
unsigned char LED9;

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = swith 1 (sw1) */
/* PA2 = swith 2 (sw2) */
/* PC[3:0] = counter1 output */
/* PC[7:0] = counter2 output */
/*---------------------------------------------------*/
void PinSetup () {
	/* Configure PA1 and PA0 as input pins to read switches */
	RCC->AHBENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	GPIOA->MODER &= ~(0x0000000F); /* General purpose input mode */
	/* Configure PC[9:0] as output pins to disply value of counters */
	RCC->AHBENR |= 0x04; /* Enable GPIOC clock (bit 2) */
	GPIOC->MODER &= ~(0x000FFFFF); /* Clear PC9-PC0 mode bits */
	GPIOC->MODER |= (0x00055555); /* General purpose output mode*/
}

void InterruptSetup	() {
	
	SYSCFG->EXTICR[0] &= 0xFFF0;

	SYSCFG->EXTICR[0] &= 0xFF0F;
	SYSCFG->EXTICR[0] |= 0x0010;
	
	EXTI->RTSR |= 0x0003;
	EXTI->IMR |= 0x0003;
	EXTI->PR |= 0x0003;
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

/*------------------------------------------------*/
/* Count function - increment/decrement counter */
/* and output to LEDs */
/*------------------------------------------------*/
void count1() {
	
		if (c1 == 9) {
			c1 = 0;
		}
		else {
			c1 = c1 + 1;
		}
		
}

void count2() {
	
	if (c2_direction == 1) { //count up
		if (c2 == 9) {
			c2 = 0;
		}
		else {
			c2 = c2 + 1;
		}
	}
	else { //count down
		if (c2 == 0) {
			c2 = 9;
		}
		else {
			c2 = c2 - 1;
		}
		
	}
	
}

void updateLEDs	() {
	
		GPIOC->BSRR |= (~c1 & 0x0F) << 16; //reset bits
		GPIOC->BSRR |= (c1 & 0x0F); //set bits
		GPIOC->BSRR |= 0xF0 << 16; //reset bits
		GPIOC->BSRR |= c2 << 4; //set bits
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

void smallDelay () {
  int i, j;
  for (i=0; i<100000; i++) { //outer loop
    i = j;; //dummy operation for single-step test
  }
}

void EXTI0_IRQHandler	() {
	EXTI->PR |= 0x0001;
	
	c2_direction = 1;
	
	if (LED8 == 1) {
		GPIOC->BSRR |= 0x0100 << 16;
		LED8 = 0;
	}
	else {
		GPIOC->BSRR |= 0x0100;
		LED8 = 1;
	}

	smallDelay();
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler	() {
	EXTI->PR |= 0x0002;
	c2_direction = 0;
	
	if (LED9 == 1) {
		GPIOC->BSRR |= 0x0200 << 16;
		LED9 = 0;
	}
	else {
		GPIOC->BSRR |= 0x0200;
		LED8 = 0;
	}
	
	smallDelay();
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}


/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/
int main(void) {
	c1 = 0; //initial counter value
	c2 = 0;
	c2_direction = 1;

	PinSetup(); //Configure GPIO 
	InterruptSetup();
	
	GPIOC->BSRR |= 0x0100 << 16;
	LED8 = 0;
	GPIOC->BSRR |= 0x0200;
	LED9 = 1;
	
	__enable_irq();
	
	/* Endless loop */
	while (1) {
		
		delay();
		count1();
		updateLEDs();
		delay();
		count1();
		count2();
		updateLEDs();
		
	} /* repeat forever */
}
