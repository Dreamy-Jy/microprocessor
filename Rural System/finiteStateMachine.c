#include "TM4C123GH6PM.h"


#define GO_NS 0x41
#define WAIT_NS 0x21
#define GO_ES 0x14
#define WAIT_ES 0x12



struct State {
	unsigned long output;
	unsigned long delay;
	unsigned long nextState[4];
};
typedef const struct State State;

const State states[4] = {
	{},
	{},
	{},
	{}
};