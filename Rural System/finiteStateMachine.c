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
void SysTick_Init(void);
void GPIO_Init(void);
void SysTick_Wait(unsigned long delay);
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
	{NS_YELLOW_EW_RED, WAIT_DELAY, {GO_NS, GO_NS, GO_NS, GO_NS}}
};

unsigned long cur_state;
unsigned long input;


int main(void) {
	//PLL_Init();
	//SysTick_Init();
	GPIO_Init();

	while (1) {
		/* code */
	}
}


void SystemInit(void) {
	SCB->CPACR |= 0x00F00000;
}


void PLL_Init(void) {}


void SysTick_Init(void) {}


void GPIO_Init(void) {
	SYSCTL->RCGCGPIO |= 0x03; // CHANGE: I may use port e instead of a
	
	GPIOB->DIR = 0x77;
	GPIOB->DEN = 0x77;
	
	
}


void SysTick_Wait(unsigned long delay) {}


void delayMs(int n) {
	int i, j;

	for(i = 0; i < n; i++)
		for(j = 0; j < 3180; j++) {}
}
