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


void PLL_Init(void);
void SysTickInit(void);
void GPIOInit(void);
void waitCycles(unsigned long cycles);
void waitMS(unsigned long delay);
void delayMs(int n);


struct State {
	unsigned long output;
	unsigned long delay;
	unsigned long nextState[4];
};


const struct State states[4] = {
	{NS_GREEN_EW_RED, GO_DELAY, {GO_NS, WAIT_NS, GO_NS, WAIT_NS}},
	{NS_YELLOW_EW_RED, WAIT_DELAY, {GO_EW, GO_EW, GO_EW, GO_EW}},
	{NS_RED_EW_GREEN, GO_DELAY, {GO_EW, GO_EW, WAIT_EW, WAIT_EW}},
	{NS_RED_EW_YELLOW, WAIT_DELAY, {GO_NS, GO_NS, GO_NS, GO_NS}}
};


unsigned long cur_state;
unsigned long input;
unsigned int state = 0;

int main(void) {
	//PLL_Init();
	SysTickInit();
	GPIOInit();
	
	while (1) {
		GPIOB->DATA = states[state].output;
		waitMS(states[state].delay);
		
		if (state < 3){
			state += 1;
		} else {
			state = 0;
		}
	}
}


void SystemInit(void) {
	__disable_irq();
	SCB->CPACR |= 0x00F00000;
}


void PLL_Init(void) {}


void SysTickInit(void) {
	SysTick->CTRL = 0;
	SysTick->CTRL = 5;
}


void GPIOInit(void) {
	SYSCTL->RCGCGPIO |= 0x03; 
	
	GPIOB->DIR = 0x77;
	GPIOB->DEN = 0x77;
	
	GPIOA->DIR = 0x00;
	GPIOA->DEN = 0x04;
	GPIOA->PDR = 0x04;
	
	GPIOA->IS &= ~0x04;
	GPIOA->IBE &= ~0x04;
	GPIOA->IEV &= ~0x04;
	GPIOA->ICR |= 0x04;
	GPIOA->IM |= 0x04;
	
	NVIC->IP[16] = 3<<5;
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
	
	GPIOB->DATA = 0x07;
	waitMS(500);
	GPIOB->DATA = 0x00;
	
	GPIOA->ICR |= 0x04;
	readback = GPIOA->ICR;
}