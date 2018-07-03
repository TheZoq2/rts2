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
const int CORE_AMOUNT = 6;

struct Fifos {
	CFifoPtr<int16_t> read_graphics;
	CFifo<int16_t, CFifo<>::r>* read_graphics_r;
	CFifo<int16_t, CFifo<>::w>* read_graphics_w;

	CFifoPtr<int16_t> read_fourier;
	CFifo<int16_t, CFifo<>::r>* read_fourier_r;
	CFifo<int16_t, CFifo<>::w>* read_fourier_w;

	CFifoPtr<std::pair<int16_t, float> > fourier_graphics;
	CFifo<std::pair<int16_t, float>, CFifo<>::r>* fourier_graphics_r;
	CFifo<std::pair<int16_t, float>, CFifo<>::w>* fourier_graphics_w;


	bool valid() {
		if(!read_graphics.valid()) {
			printf("read->graphics fifo was invalid\n");
			return false;
		}
		if(!read_fourier.valid()) {
			printf("read->fourier fifo was invalid\n");
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
