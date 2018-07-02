#include "graphics_thread.h"

#include <stdio.h>
#include <helix.h>
#include <render/render.h>
#include <time.h>

#include "cores.h"
#include "data.h"
#include "defines.h"
#include "fourier_params.h"

const int SELF = GRAPHICS_CORE;

const int DATA_BUFFER_SIZE = 640;

void draw_values(int* values, int start, int length, int y_offset) {
	// Redraw all the data
	int prev_x = 0;
	int prev_y = 0;
	for (int i = 0; i < length; ++i) {
		int index = (start + i) % length;

		drawline(prev_x, prev_y + y_offset, i, values[index] + y_offset, white, 0, 0);
		prev_x = i;
		prev_y = values[index];
	}
}

void* graphics_thread(void* args) {
	Fifos* fifos = (Fifos*) args;
	printf("graphics thread started\n");

	// Setup display
	if(render_init(1) != RENDER_OK) {
		printf("Failed to init renderer");
		return 0;
	}

	// Fill the background with black
	fillrect(0, 0, 1000, 1000, black);
	render_flip_buffer();

	int data_buffer[DATA_BUFFER_SIZE];

	// Clear the data for the first run
	for(int i = 0; i < DATA_BUFFER_SIZE; ++i) {
		data_buffer[i] = 0;
	}

	int fourier_current_offset = 0;
	int fourier_reals[FOURIER_SIZE];
	

	int current_index = 0;

	while(true) {
		// Read data from the producer thread
		while(!fifos->r[READ_CORE][SELF]->empty()) {
			Data data = fifos->r[READ_CORE][SELF]->front();
			fifos->r[READ_CORE][SELF]->pop();

			if(data.type == RawReading) {
				data_buffer[current_index] = data.value;
				current_index = (current_index + 1) % DATA_BUFFER_SIZE;
			}
			else {
				printf("Graphics thread: Got unexpected datatype %i\n", data.type);
			}
		}

		// Read data from the fourier thread
		while(!fifos->r[FOURIER_CORE][SELF]->empty()) {
			Data data = fifos->r[FOURIER_CORE][SELF]->front();
			fifos->r[FOURIER_CORE][SELF]->pop();

			if(data.type == FFT_Sync) {
				fourier_current_offset = 0;
			}
			else if(data.type == FFT_Real) {
				if (fourier_current_offset < FOURIER_SIZE) {
					fourier_reals[fourier_current_offset] = data.value;
					fourier_current_offset += 1;
				}
			}
			else if(data.type == FFT_Img) {

			}
			else {
				printf("Graphics thread: Got unexpected datatype %i\n", data.type);
			}
		}

		fillrect(0, 0, 640, 480, black);

		// Be careful with printfs here, it will slow the loop down significantly
		draw_values(data_buffer, current_index, DATA_BUFFER_SIZE, 0);
		draw_values(fourier_reals, 0, FOURIER_SIZE, 200);


		render_flip_buffer();
	}
	return 0;
}
