#include "read_thread.h"
#include <stdio.h>

#include "cores.h"
#include "data.h"

const int adc_delay_us = 1;

const int SELF = READ_CORE;

int square_wave(int iteration) {
	if (iteration % 150 > 75) {
		return 100;
	}
	else {
		return 0;
	}
}

int sawtooth(int iteration) {
	return (iteration / 2) % 100;
}

void* read_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Read thread running\n");

	int i = 0;
	while(true) {
		usleep(adc_delay_us);
		fifos->w[SELF][GRAPHICS_CORE]->push(Data(sawtooth(i), RawReading));
		fifos->w[SELF][GRAPHICS_CORE]->push(Data(sawtooth(i), RawReading));
		i++;
	}
	return 0;
}
