#include "graphics_thread.h"

#include <stdio.h>
#include <helix.h>
#include <render/render.h>
#include <time.h>

#include "cores.h"
#include "data.h"
#include "defines.h"

const int SELF = GRAPHICS_CORE;

const int DATA_BUFFER_SIZE = 640;

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

	int current_index = 0;

	while(true) {
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
		// Be careful with printfs here, it will slow the loop down significantly

		// Redraw all the data
		int prev_x = 0;
		int prev_y = 0;
		fillrect(0, 0, 1000, 1000, black);
		for (int i = 0; i < DATA_BUFFER_SIZE; ++i) {
			int index = (current_index + i) % DATA_BUFFER_SIZE;

			drawline(prev_x, prev_y, i, data_buffer[index], white, 0, 0);
			prev_x = i;
			prev_y = data_buffer[index];
			// printf("%i\n", data_buffer[index]);
		}

		render_flip_buffer();
	}
	return 0;
}
