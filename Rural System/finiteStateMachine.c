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

// Delays in milliseconds
#define GO_DELAY 500
#define WAIT_DELAY 500

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

void PLL_Init(void);
void SysTickInit(void);
void GPIOInit(void);
void waitCycles(unsigned long cycles);
void waitMS(unsigned long delay);
void delayMs(int n);


const struct State states[4] = {
	{NS_GREEN_EW_RED, GO_DELAY, {GO_NS, WAIT_NS}},
	{NS_YELLOW_EW_RED, WAIT_DELAY, {GO_EW, GO_EW}},
	{NS_RED_EW_GREEN, GO_DELAY, {GO_EW, WAIT_EW}},
	{NS_RED_EW_YELLOW, WAIT_DELAY, {GO_NS, GO_NS}}
};


int blockChange = 0;
unsigned int state = 0;


int main(void) {
	SysTickInit();
	GPIOInit();
	
	while (1) {
		GPIOB->DATA = states[state].output;
		
		if(blockChange == 1) {
			GPIOA->IM = 0x00;
			waitMS(states[state].delay);
			blockChange = 0;
			GPIOA->IM |= 0x88;
		} else {
			state = states[state].nextState[0];
		}
		
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
	SYSCTL->RCGCGPIO |= 0x02; 
	
	GPIOB->DIR = 0x77;
	GPIOB->DEN = 0x77;
	
	GPIOA->DIR = 0x00;
	GPIOA->DEN = 0x0A;
	GPIOA->PDR = 0x0A;
	
	GPIOA->IS &= ~0x0A;
	GPIOA->IBE &= ~0x0A;
	GPIOA->IEV &= ~0x0A;
	GPIOA->ICR |= 0x0A;
	GPIOA->IM |= 0x0A;
	
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
		waitCycles(16000); // 80000 or 16000
	}
}


void GPIOA_Handler(void) {
	volatile int readback;
	
	state = states[state].nextState[1];
	blockChange = 1;

	GPIOA->ICR |= 0x04;
	readback = GPIOA->ICR;
}