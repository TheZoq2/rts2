#include "graphics_thread.h"
#include <stdio.h>
#include <helix.h>
#include <render/render.h>
#include <time.h>

#include "cores.h"
#include "data.h"
#include "defines.h"
#include "fourier_params.h"
#include "graphics_util.h"

const int SELF = GRAPHICS_CORE;

const int DATA_BUFFER_SIZE = 500;

void* graphics_thread(void* args) {
	Fifos* fifos = (Fifos*) args;
	printf("graphics thread started\n");

	int data_buffer[DATA_BUFFER_SIZE];

	// Clear the data for the first run
	for(int i = 0; i < DATA_BUFFER_SIZE; ++i) {
		data_buffer[i] = 0;
	}
	

	int current_index = 0;

	while(true) {
		// Read data from the producer thread
		if(!fifos->trigger_graphics_r->empty()) {
			int value = fifos->trigger_graphics_r->front();
			fifos->trigger_graphics_r->pop();

			data_buffer[current_index] = value;
			current_index = (current_index + 1) % DATA_BUFFER_SIZE;
		}

		printf("Graphics thread is running\n");

		// If it is time to render another frame
		if (!fifos->gcommand_graphics_r->empty()) {
			printf("graphics: Starting render\n");
			fifos->gcommand_graphics_r->pop();
			fillrect(0, 0, 640, 200, black);

			draw_values(data_buffer, current_index, DATA_BUFFER_SIZE, 100, 1);

			printf("graphics: Telling graphics command that drawing is done\n");
			fifos->graphics_gcommand_w->push(true);
			printf("graphics: Told graphics command that drawing is done\n");
		}

	}
	return 0;
}
