#pragma once
#ifndef H_CORES
#define H_CORES

#include <helix.h>

#include "data.h"

const int DUMMY_CORE = 0;
const int READ_CORE = 1;
const int GRAPHICS_CORE = 2;
const int FOURIER_CORE = 3;
const int PEAK_TO_PEAK_CORE = 4;
const int CORE_AMOUNT = 5;

struct Fifos {
	CFifo<Data, CFifo<>::r>* r[CORE_AMOUNT][CORE_AMOUNT];
	CFifo<Data, CFifo<>::w>* w[CORE_AMOUNT][CORE_AMOUNT];
};

#endif
