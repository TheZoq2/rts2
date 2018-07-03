#include "read_thread.h"
#include <stdio.h>
#include <math.h>

#include "cores.h"
#include "data.h"

const int adc_delay_us = 5;

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

int sin_wave(int iteration) {
	return 100 * sin(iteration / 4. * 3.14);
}

int two_sin(int iteration) {
	return 50 * sin(iteration / 4. * 3.14) + 50 * sin(iteration / 16. * 3.14);
}

void* read_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Read thread running\n");

	int i = 0;
	while(true) {
		usleep(adc_delay_us);
		int value = sawtooth(i);
		fifos->read_graphics_w->push(value);
		fifos->read_fourier_w->push(value);
		i++;
	}
	return 0;
}
