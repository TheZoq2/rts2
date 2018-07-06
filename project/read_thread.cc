#include "read_thread.h"
#include <stdio.h>
#include <math.h>

#include "cores.h"
#include "data.h"
#include "util.h"

const int adc_delay_us = 1;

int square_wave(int time) {
	if (time % 10000 > 5000) {
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
	return 100 * sin(iteration / (18000.) * 3.14);
}

int two_sin(int iteration) {
	return 50 * sin(iteration / 26000. * 3.14) + 50 * sin(iteration / 120000. * 3.14);
}

int random_signal(int i) {
	return (rand() % 200) - 100;
}

int random_and_square_wave(int i) {
	return random_signal(i) / 4 + square_wave(i) * 3/4.;
}

int periodic(int i, int (*main_function)(int), int period, int length) {
	if(i % period < length) {
		return main_function(i);
	}
	else {
		return 0;
	}
}

int with_noise(int i, int (*main_function)(int), float snr) {
	return main_function(i) * (1-snr) + random_signal(i) * snr;
}

const int MAX_READ_RATE = 2000;
void* read_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Read thread running\n");

	unsigned int elapsed_time = 0;
	unsigned int last_sample = get_microseconds();
	int i = 0;
	while(true) {
		unsigned int now = get_microseconds();
		if(now - last_sample > MAX_READ_RATE) {
			unsigned int last_sample = now;
			// int value = periodic(get_microseconds(), random_and_square_wave, 1000000, 10000);
			int value = with_noise(now, two_sin, 1/4.);
			// int value = with_noise(now, sin_wave, 1/4.);
			// int value = sin_wave(i);
			fifos->read_trigger_w->push(value);
			i++;
		}
	}
	return 0;
}
