#include "fourier_thread.h"

#include <stdio.h>
#include <utility>

#include "data.h"
#include "cores.h"
#include "fourier_params.h"
#include "util.h"

// Use fixed point fft
// #define FIXED_POINT 32
#include "kissfft/kiss_fftr.h"


const int SELF = FOURIER_CORE;

void* fourier_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Fourier thread running\n");

	// Setup a fft struct with NFFT samples which does not invert and with no preallocated memory
	kiss_fftr_cfg cfg = kiss_fftr_alloc(NFFT, 0, NULL, NULL);

	while(true) {
		int buffer_index = 0;
		float buffer[NFFT];

		// Get data from the reader thread
		while(buffer_index < NFFT) {
			// Read data from the fifo
			int val = fifos->trigger_fourier_r->front();
			fifos->trigger_fourier_r->pop();

			buffer[buffer_index] = val / 100.;
			buffer_index += 1;
		}

		// Run the fourier transform
		
		unsigned int start = get_microseconds();
		kiss_fft_cpx fft_buffer[FOURIER_SIZE];
		kiss_fftr(cfg, buffer, fft_buffer);
		unsigned int time = get_microseconds() - start;
		printf("fourier processing time %i\n", time);

		for (int i = 0; i < FOURIER_SIZE; ++i) {
			float abs_value = sqrt(pow(fft_buffer[i].r, 2) + pow(fft_buffer[i].i, 2));
			fifos->fourier_fg_w->push(std::make_pair(i, abs_value * 100 / (FOURIER_SIZE / 2)));
		}
	}

	// Just to be sure, deallocate the fft config struct
	free(cfg);
	return 0;
}
