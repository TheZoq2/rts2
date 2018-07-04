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

const int DATA_BUFFER_SIZE = 500;

void draw_values(int* values, int start, int length, int y_offset, int x_step, int x_start=0) {
	// Redraw all the data
	int prev_x = 0;
	int prev_y = values[0];
	for (int i = 0; i < length; ++i) {
		int index = (start + i) % length;

		drawline(x_start + prev_x * x_step, -prev_y + y_offset, x_start + i * x_step, -values[index] + y_offset, white, 0, 0);
		prev_x = i;
		prev_y = values[index];
	}
}

unsigned int get_microseconds() {
	unsigned int val = get_clock();

	int seconds = val >> 20;
	int microseconds = val & 0xfffff;

	return seconds * 1000000 + microseconds;
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

	unsigned int last_frame = get_microseconds();
	unsigned int last_render_time = 0;
	int frame_times[100] = {0};
	int frame_time_offset = 0;

	while(true) {
		// Read data from the producer thread
		if(!fifos->trigger_graphics_r->empty()) {
			int value = fifos->trigger_graphics_r->front();
			fifos->trigger_graphics_r->pop();

			data_buffer[current_index] = value;
			current_index = (current_index + 1) % DATA_BUFFER_SIZE;
		}

		// Read data from the fourier thread
		if(!fifos->fourier_graphics_r->empty()) {
			std::pair<int, float> data = fifos->fourier_graphics_r->front();
			fifos->fourier_graphics_r->pop();

			fourier_reals[data.first] = data.second;
		}

		unsigned int loop_start = get_microseconds();

		// If it is time to render another frame
		if ((get_microseconds() - last_frame) > 5000) {
			fillrect(0, 0, 640, 480, black);

			draw_values(data_buffer, current_index, DATA_BUFFER_SIZE, 100, 1);
			draw_values(fourier_reals, 0, FOURIER_SIZE, 450, 3);
			draw_values(frame_times, frame_time_offset, 100, 100, 1, 500);

			char text_buffer[32];
			sprintf(text_buffer, "Frame time (ms)%f", last_render_time / 1000.);
			drawstring(500, 10, text_buffer, red);

			render_flip_buffer();
			last_frame = get_microseconds();

			last_render_time = last_frame - loop_start;
			frame_times[frame_time_offset] = last_render_time / 1000.;
			frame_time_offset = (frame_time_offset + 1) % 100;
		}

	}
	return 0;
}
