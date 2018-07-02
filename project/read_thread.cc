#include "read_thread.h"
#include <stdio.h>

#include "cores.h"
#include "data.h"

const int adc_delay = 1000;

const int SELF = READ_CORE;

void fakesleep(int delay) {
	for(int i = 0; i < delay; i++) {}
}

int square_wave(int iteration) {
	if (iteration % 1000 > 500) {
		return 100;
	}
	else {
		return 0;
	}
}

void* read_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Read thread running\n");

	int i = 0;
	while(true) {
		fifos->w[SELF][GRAPHICS_CORE]->push(Data(square_wave(i), RawReading));
		fakesleep(adc_delay);
		i++;
	}
	return 0;
}
