#include "TM4C123GH6PM.h"

/**
Why use unsiged long for state?
How does memeory management work in C++?
*/

// Output States for lights, using port B
#define NS_GREEN_EW_RED 0x41
#define NS_YELLOW_EW_RED 0x21
#define NS_RED_EW_GREEN 0x14
#define NS_RED_EW_YELLOW 0x12
#define NS_YELLOW_EW_YELLOW 0x22
#define NS_RED_EW_RED 0x11

// Delays in milliseconds
#define GO_DELAY 2000
#define WAIT_DELAY 3000

// State values
#define GO_NS 0
#define WAIT_NS 1
#define GO_EW  2
#define WAIT_EW 3

struct State {
	unsigned long output;
	unsigned long delay;
	unsigned long nextState[2];
};

void SysTickInit(void);
void GPIOInit(void);
void waitCycles(unsigned long cycles);
void waitMS(unsigned long delay);


const struct State states[4] = {
	{NS_GREEN_EW_RED, GO_DELAY, {GO_NS, WAIT_NS}},
	{NS_YELLOW_EW_RED, WAIT_DELAY, {GO_EW, GO_EW}},
	{NS_RED_EW_GREEN, GO_DELAY, {GO_EW, WAIT_EW}},
	{NS_RED_EW_YELLOW, WAIT_DELAY, {GO_NS, GO_NS}},
};


int blockChange = 0;
unsigned int state = 0;
unsigned int next = 0;


int main(void) {
	SysTickInit();
	GPIOInit();
	
	while (1) {
		GPIOB->DATA = states[state].output;
		
		if(blockChange == 1) {
			GPIOA->IM &= ~(0x1C);
			
			waitMS(states[state].delay);
			blockChange = 0;
			next = 0;
			
			GPIOA->IM |= 0x1C;
		}
		
		state = states[state].nextState[next];
	}
}


void SystemInit(void) {
	__disable_irq();
	SCB->CPACR |= 0x00F00000;
}


void SysTickInit(void) {
	SysTick->CTRL = 0;
	SysTick->CTRL = 5;
}


void GPIOInit(void) {
	SYSCTL->RCGCGPIO |= 0x03; 
	
	GPIOB->DIR = 0x77;
	GPIOB->DEN = 0x77;
	
	GPIOA->DIR = 0x00;
	GPIOA->DEN = 0x1C;
	GPIOA->PDR = 0x1C;
	
	GPIOA->IS &= ~0x1C;
	GPIOA->IBE &= ~0x1C;
	GPIOA->IEV |= 0x1C;
	GPIOA->ICR |= 0x1C;
	GPIOA->IM |= 0x1C;
	
	NVIC->IP[0] = 3<<5;
	NVIC->ISER[0] |= 0x00000001;
	
	__enable_irq();
}


void waitCycles(unsigned long cycles) {
	SysTick->LOAD = cycles-1;
	SysTick->VAL = 0;
	
	while((SysTick->CTRL & 0x00010000) == 0){}
}


void waitMS(unsigned long delay) {
	unsigned long i;
	
	for(i=0; i<delay; i++) {
		waitCycles(16000);
	}
}


void GPIOA_Handler(void) {
	volatile int readback;
	
	if ((GPIOA->DATA & 0x10) == 0x10) {
		GPIOB->DATA = NS_YELLOW_EW_YELLOW;
		waitMS(WAIT_DELAY);
		GPIOB->DATA = NS_RED_EW_RED;
		waitMS(WAIT_DELAY);
		GPIOB->DATA = states[state].output;
	} else if((state == GO_EW && (GPIOA->DATA & 0x08) == 0x08) || (state == GO_NS && (GPIOA->DATA & 0x04) == 0x04)) {
		next = 1;
		blockChange = 1;
	}

	GPIOA->ICR |= 0x1C;
	readback = GPIOA->ICR;
}