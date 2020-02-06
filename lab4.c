/*====================================================*/
/* Zachary Leggett and Santhosh Thundena */
/* ELEC 3040/3050 - Lab 4 */
/* Two decade counters controlled that both initial count up. */
/* Counter 1 has a delay of 0.5 sec and Counter 2 has a delay of 1 sec. */
/* Pushing button PA0 triggers interrupt EXTI0 that sets Counter 2 to count down and toggles LED8 */
/* Pushing button PA1 triggers interrupt EXTI1 that sets Counter 2 to count up and toggles LED9. */
/* Counter 1 is diaplayed on LEDs 3-0 and Counter 2 is displayed on LEDs 7-4. */
/*====================================================*/

#include "STM32L1xx.h" /* Microcontroller information */

/* Define global variables */
unsigned char c1; // value of first counter
unsigned char c2; // value of second counter
unsigned char c2_direction; // direction of second counter
unsigned char LED8; // state of LED8 which is toggled by EXTI0
unsigned char LED9; // state of LED9 which is toggled by EXTI1

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA0 triggers interrupt EXTI0 */
/* PA1 triggers interrupt EXTI1 */
/* PC[3:0] = counter1 output */
/* PC[7:4] = counter2 output */
/* PC[8] = LED8 for EXTI0 */
/* PC[9] = LED9 for EXTI1 */
/*---------------------------------------------------*/
void PinSetup() {
	/* Configure PA1 and PA0 as input pins for buttons */
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	GPIOA->MODER &= ~(0x0000000F); // General purpose input mode
	/* Configure PC[9:0] as output pins to disply value of counters */
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 2)
	GPIOC->MODER &= ~(0x000FFFFF); // Clear PC9-PC0 mode bits
	GPIOC->MODER |= (0x00055555); // General purpose output mode
}

void InterruptSetup() {
	
	SYSCFG->EXTICR[0] &= 0xFFF0; // Configure EXTI0 to be triggered by PA0

	SYSCFG->EXTICR[0] &= 0xFF0F; // Clear EXTI1
	SYSCFG->EXTICR[0] |= 0x0010; // Configure EXTI1 to be triggered by PA1
	
	EXTI->RTSR |= 0x0003; // Set EXTI0 and EXTI1 to be rising edge triggered
	EXTI->IMR |= 0x0003; // Set interrupt masks for EXTI0 and EXTI1
	EXTI->PR |= 0x0003; // Set pending register for EXTI0 and EXTI1
	
	NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0
	NVIC_EnableIRQ(EXTI1_IRQn); // Enable EXTI1
	
	NVIC_ClearPendingIRQ(EXTI0_IRQn); // Clear pending register for EXTI0
	NVIC_ClearPendingIRQ(EXTI1_IRQn); // Clear pending register for EXTI1
}

/*------------------------------------------------*/
/* Count1 function - increment counter 1 */
/*------------------------------------------------*/
void count1() {
	
		if (c1 == 9) { 
			c1 = 0;   // Sets Counter 1 to 0 if currently at 9
		}
		else {
			c1 = c1 + 1;  // Increments Counter 1
		}
		
}

/*------------------------------------------------*/
/* Count2 function - increment/decrement counter 2 */
/*------------------------------------------------*/
void count2() {
	
	if (c2_direction == 1) { //count up
		if (c2 == 9) {
			c2 = 0;  // Sets Counter 2 to 0 if currently at 9
		}
		else {
			c2 = c2 + 1;  // Increments Counter 2
		}
	}
	else { //count down
		if (c2 == 0) {
			c2 = 9;  // Sets Counter 2 to 9 if currently at 0
		}
		else {
			c2 = c2 - 1;  // Decrements Counter 2
		}
	}
	
}

void updateLEDs() {
	
		GPIOC->BSRR |= (~c1 & 0x0F) << 16; //reset bits for Counter 1
		GPIOC->BSRR |= (c1 & 0x0F); //set bits for Counter 1
		GPIOC->BSRR |= 0xF0 << 16; //reset bits for Counter 2
		GPIOC->BSRR |= c2 << 4; //set bits for Counter 2
}

/*----------------------------------------------------------*/
/* Delay function - do nothing for about 0.5 second */
/*----------------------------------------------------------*/
void delay() {
	int i,j,n;
	for (i=0; i<10; i++) { //outer loop
		for (j=0; j<20000; j++) { //inner loop
			n = j; //dummy operation for single-step test
		} //do nothing
 }
}

/*----------------------------------------------------------*/
/* SmallDelay function - do nothing for a short period */
/*----------------------------------------------------------*/
void smallDelay() {
  int i, j;
  for (i=0; i<100000; i++) { //outer loop
    i = j;; //dummy operation for single-step test
  }
}

/*----------------------------------------------------------*/
/* EXTI1_IRQHandler function - performs operations when EXTI0 is triggered */
/*----------------------------------------------------------*/
void EXTI0_IRQHandler() {
	EXTI->PR |= 0x0001; //Set pending register for EXTI0
	
	c2_direction = 0; // Set Counter 1 to count down
	
	if (LED8 == 1) {
		GPIOC->BSRR |= 0x0100 << 16; // Turn LED8 off
		LED8 = 0;
	}
	else {
		GPIOC->BSRR |= 0x0100; // Turn LED8 on
		LED8 = 1;
	}

	smallDelay();
	NVIC_ClearPendingIRQ(EXTI0_IRQn); //Reset pending register for EXTI0
}

/*----------------------------------------------------------*/
/* EXTI1_IRQHandler function - performs operations when EXTI1 is triggered */
/*----------------------------------------------------------*/
void EXTI1_IRQHandler() {
	EXTI->PR |= 0x0002; //Set pending register for EXTI1
	
	c2_direction = 1; //Set Counter 2 to count up
	
	if (LED9 == 1) {
		GPIOC->BSRR |= 0x0200 << 16; //Turn LED9 off
		LED9 = 0;
	}
	else {
		GPIOC->BSRR |= 0x0200; // Turn LED9 on
		LED9 = 1;
	}
	
	smallDelay();
	NVIC_ClearPendingIRQ(EXTI1_IRQn); //Reset pending register for EXTI1
}


/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/
int main(void) {
	c1 = 0; // Initial Counter 1 value
	c2 = 0; // Initial Counter 2 value
	c2_direction = 1; // Inital Counter 2 direction of up

	PinSetup(); // Configure GPIO 
	InterruptSetup(); // Configure interrupts
	
	GPIOC->BSRR |= 0x0100 << 16; // Set LED8 to initially be off
	LED8 = 0;
	GPIOC->BSRR |= 0x0200; // Set LED9 to initially be on
	LED9 = 1;
	
	__enable_irq(); //Enable interrupts to occur
	
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
