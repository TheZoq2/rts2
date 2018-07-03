#include "fourier_thread.h"

#include <stdio.h>

#include "data.h"
#include "cores.h"
#include "fourier_params.h"

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
			int val = fifos->read_fourier_r->front();
			fifos->read_fourier_r->pop();

			buffer[buffer_index] = val / 100.;
			buffer_index += 1;
		}

		// Run the fourier transform
		
		kiss_fft_cpx fft_buffer[FOURIER_SIZE];
		kiss_fftr(cfg, buffer, fft_buffer);

		float* output_buffer = new float[FOURIER_SIZE];
		// Convert to amplitude values
		for(int i = 0; i < FOURIER_SIZE; ++i) {
			output_buffer[i] = sqrt(pow(fft_buffer[i].r, 2) + pow(fft_buffer[i].i, 2));
		}

		fifos->fourier_graphics_w->push(output_buffer);
	}

	// Just to be sure, deallocate the fft config struct
	free(cfg);
	return 0;
}
