#pragma once
#ifndef H_CORES
#define H_CORES

#include <helix.h>

#include "data.h"
// #include "types.h"

const int DUMMY_CORE = 0;
const int READ_CORE = 1;
const int GRAPHICS_CORE = 2;
const int FOURIER_CORE = 3;
const int PEAK_TO_PEAK_CORE = 4;
const int FOURIER_BUFFER_CORE = 5;
const int TRIGGER_CORE = 6;
const int COMMAND_CORE = 7;
const int CORE_AMOUNT = 8;

struct Fifos {
	CFifoPtr<int16_t> read_trigger;
	CFifo<int16_t, CFifo<>::r>* read_trigger_r;
	CFifo<int16_t, CFifo<>::w>* read_trigger_w;

	CFifoPtr<int16_t> trigger_graphics;
	CFifo<int16_t, CFifo<>::r>* trigger_graphics_r;
	CFifo<int16_t, CFifo<>::w>* trigger_graphics_w;

	CFifoPtr<int16_t> trigger_fourier;
	CFifo<int16_t, CFifo<>::r>* trigger_fourier_r;
	CFifo<int16_t, CFifo<>::w>* trigger_fourier_w;

	CFifoPtr<std::pair<int16_t, float> > fourier_graphics;
	CFifo<std::pair<int16_t, float>, CFifo<>::r>* fourier_graphics_r;
	CFifo<std::pair<int16_t, float>, CFifo<>::w>* fourier_graphics_w;


	bool valid() {
		if(!read_trigger.valid()) {
			printf("read->trigger fifo was invalid\n");
			return false;
		}
		if(!trigger_graphics.valid()) {
			printf("trigger->graphics fifo was invalid\n");
			return false;
		}
		if(!trigger_fourier.valid()) {
			printf("trigger->fourier fifo was invalid\n");
			return false;
		}
		if(!fourier_graphics.valid()) {
			printf("fourier->graphics fifo was invalid\n");
			return false;
		}

		return true;
	}
};

#endif
