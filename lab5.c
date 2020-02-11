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

struct {
	unsigned char row;
	unsigned char column;
	unsigned char event;
	const unsigned char row1[4];
  const unsigned char row2[4];
  const unsigned char row3[4];
  const unsigned char row4[4];
  const unsigned char* keys[];
} typedef keypad;


keypad keypad1 = {
  .row = ~0,  
  .column = ~0, 
  .event = 0,
  .row1 = {1, 2, 3, 0xA},
  .row2 = {4, 5, 6, 0xB},
  .row3 = {7, 8, 9, 0xC},
  .row4 = {0xE, 0, 0xF, 0xD},
  .keys = {keypad1.row1, keypad1.row2, keypad1.row3, keypad1.row4},
};

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
	/* Configure PA1 as input */
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	GPIOA->MODER &= ~(0x0000000C); // General purpose input mode
	
	/* Configure PB7-0 as inputs */
	RCC->AHBENR |= 0x02; // Enable GPIOB clock (bit 1)
	GPIOB->MODER &= ~(0x0000FFFF);
	
	GPIOB->PUPDR &= ~(0x000000FF);
	GPIOB->PUPDR |= (0x00000055);
	
	GPIOB->PUPDR &= ~(0x0000FF00);
	GPIOB->PUPDR |= (0x00005500);
	
	/* Configure PC[3:0] as output pins to disply value of counter */
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 2)
	GPIOC->MODER &= ~(0x000000FF); // Clear PC3-PC0 mode bits
	GPIOC->MODER |= (0x00000055); // General purpose output mode
}

void InterruptSetup() {
	
	SYSCFG->EXTICR[0] &= 0xFF0F; // Clear EXTI1
	SYSCFG->EXTICR[0] |= 0x0010; // Configure EXTI1 to be triggered by PA1
	
	EXTI->FTSR |= 0x0002; // Set EXTI0 and EXTI1 to be rising edge triggered
	EXTI->IMR |= 0x0002; // Set interrupt masks for EXTI0 and EXTI1
	EXTI->PR |= 0x0002; // Set pending register for EXTI0 and EXTI1

	NVIC_EnableIRQ(EXTI1_IRQn); // Enable EXTI1
	
	NVIC_ClearPendingIRQ(EXTI1_IRQn); // Clear pending register for EXTI1
}



void updateLEDs(unsigned char count) {
	
		GPIOC->BSRR |= (~count & 0x0F) << 16; //reset bits
		GPIOC->BSRR |= (count & 0x0F); //set bits
}

/*----------------------------------------------------------*/
/* Delay function - do nothing for about 1 second */
/*----------------------------------------------------------*/
void delay () {
	int i,j,n;
	for (i=0; i<20; i++) { //outer loop
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
  for (i=0; i<4; i++) { //outer loop
    i = j;; //dummy operation for single-step test
  }
}

/*----------------------------------------------------------*/
/* EXTI1_IRQHandler function - performs operations when EXTI1 is triggered */
/*----------------------------------------------------------*/
void EXTI1_IRQHandler() {
	EXTI->PR |= 0x0002; //Set pending register for EXTI1
	
	const int COLUMN_MASK[] = {(GPIOB->BSRR & 0x10), (GPIOB->BSRR & 0x20), (GPIOB->BSRR & 0x40), (GPIOB->BSRR & 0x80)};
	const int ROW_MASK[] = {(GPIOB->IDR & 0x01), (GPIOB->IDR & 0x02), (GPIOB->IDR & 0x04), (GPIOB->IDR & 0x08)};
	
	for (keypad1.column=0; keypad1.column < 4; keypad1.column++) {
		GPIOB->BSRR |= 0x000000F0;
		GPIOB->BSRR &= COLUMN_MASK[keypad1.column];
		
		for (keypad1.row=0; keypad1.row < 4; keypad1.row++) {
			smallDelay();
			unsigned short temp = (GPIOB->IDR & ROW_MASK[keypad1.row]);
			
			if (!temp) {
				keypad1.event = 4;
				updateLEDs(keypad1.keys[keypad1.row][keypad1.column]);
				GPIOB->BSRR |= 0x00F00000;
				NVIC_ClearPendingIRQ(EXTI1_IRQn);
				return;
			}
		}
	}
	
	GPIOB->BSRR |= 0x00F00000;
	NVIC_ClearPendingIRQ(EXTI1_IRQn); //Reset pending register for EXTI1
}


/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/
int main(void) {
	
	PinSetup(); // Configure GPIO 
	InterruptSetup(); // Configure interrupts
	
	unsigned char count = 0;
	
	GPIOB->BSRR |= 0x00F00000; // Ground columns

	
	__enable_irq(); //Enable interrupts to occur
	
	/* Endless loop */
	while (1) {
		
		delay();
		
		if (count == 9) { 
			count = 0;   // Sets Count to 0 if currently at 9
		}
		else {
			count = count + 1;  // Increments Count
		}
		
		if (keypad1.event) {
			keypad1.event--;
		}
		else {
			updateLEDs(count);
		}
		
		
	} /* repeat forever */
}
