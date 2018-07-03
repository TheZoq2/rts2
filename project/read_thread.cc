#include "read_thread.h"
#include <stdio.h>
#include <math.h>

#include "cores.h"
#include "data.h"

const int adc_delay_us = 1;

int square_wave(int iteration) {
	if (iteration % 128 > 64) {
		return 100;
	}
	else {
		return -100;
	}
}

int sawtooth(int iteration) {
	return ((iteration * 4) % 200) - 100;
}

int sin_wave(int iteration) {
	return 100 * sin(iteration / 4. * 3.14);
}

int two_sin(int iteration) {
	return 50 * sin(iteration / 4. * 3.14) + 50 * sin(iteration / 16. * 3.14);
}

int random_signal(int i) {
	return (rand() % 200) - 100;
}

int random_and_square_wave(int i) {
	return random_signal(i) / 4 + square_wave(i);
}

void* read_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Read thread running\n");

	int i = 0;
	while(true) {
		usleep(adc_delay_us);
		int value = random_and_square_wave(i);
		fifos->read_graphics_w->push(value);
		fifos->read_fourier_w->push(value);
		i++;
	}
	return 0;
}
