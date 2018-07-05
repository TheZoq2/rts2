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
#include "util.h"

const int SELF = GRAPHICS_CORE;

const int DATA_BUFFER_SIZE = 250;

void* graphics_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	// Validate all fifos that are used by this thread
	validate_fifo(fifos->trigger_graphics_r, "trigger_graphics_r");
	validate_fifo(fifos->gcommand_graphics_r, "gcommand_graphics_r");
	validate_fifo(fifos->graphics_gcommand_w, "graphics_gcommand_w");

	printf("graphics thread started\n");

	int data_buffer[DATA_BUFFER_SIZE];

	// Clear the data for the first run
	for(int i = 0; i < DATA_BUFFER_SIZE; ++i) {
		data_buffer[i] = 0;
	}
	

	int current_index = 0;

	while(true) {
		// Read data from the producer thread
		if(!fifos->trigger_graphics_r->empty() && !fifos->gcommand_graphics_r->empty()) {
			unsigned int draw_start = get_microseconds();
			Datachunk data = fifos->trigger_graphics_r->front();
			fifos->trigger_graphics_r->pop();
			fifos->gcommand_graphics_r->pop();

			fillrect(0, 0, 500, 200, black);
			draw_values(data.data, CHUNK_SIZE-DATA_BUFFER_SIZE, DATA_BUFFER_SIZE, 100, 1, 0);

			// FlushDCache();
			render_flush();

			unsigned int draw_time = get_microseconds() - draw_start;

			// printf("%i\n", draw_time);
			fifos->graphics_gcommand_w->push(true);
		}
	}
	return 0;
}
