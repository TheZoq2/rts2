#include "fourier_thread.h"

#include <stdio.h>

#include "data.h"
#include "cores.h"
#include "fourier_params.h"

// Use fixed point fft
#define FIXED_POINT 32
#include "kissfft/kiss_fftr.h"


const int SELF = FOURIER_CORE;

void* fourier_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	printf("Fourier thread running\n");

	// Setup a fft struct with NFFT samples which does not invert and with no preallocated memory
	kiss_fftr_cfg cfg = kiss_fftr_alloc(NFFT, 0, NULL, NULL);

	while(true) {
		int buffer_index = 0;
		int32_t buffer[NFFT];

		// Get data from the reader thread
		while(!fifos->r[READ_CORE][SELF]->empty() && buffer_index < NFFT) {
			// Read data from the fifo
			Data data = fifos->r[READ_CORE][SELF]->front();
			fifos->r[READ_CORE][SELF]->pop();

			// Put it in the buffer
			if(data.type == RawReading) {
				buffer[buffer_index] = data.value;
				buffer_index += 1;
			}
			else {
				printf("Fourier thread got unexpected data from reader thread. Type: %i\n", data.type);
			}
		}

		// Run the fourier transform
		
		kiss_fft_cpx fft_buffer[FOURIER_SIZE];
		kiss_fftr(cfg, buffer, fft_buffer);

		for(int i = 0; i < FOURIER_SIZE; ++i) {
			// Send the fourier data to the graphics core
			fifos->w[SELF][GRAPHICS_CORE]->push(Data(fft_buffer[i].r, FFT_Real));
			fifos->w[SELF][GRAPHICS_CORE]->push(Data(fft_buffer[i].i, FFT_Img));
		}
	}

	// Just to be sure, deallocate the fft config struct
	free(cfg);
	return 0;
}
