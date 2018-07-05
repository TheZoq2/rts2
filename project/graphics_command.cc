#include "graphics_command.h"

#include "cores.h"
#include "util.h"

unsigned int get_microseconds() {
	unsigned int val = get_clock();

	int seconds = val >> 20;
	int microseconds = val & 0xfffff;

	return seconds * 1000000 + microseconds;
}

void* graphics_command_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	validate_fifo(fifos->graphics_gcommand_r, "graphics_gcommand_r");
	validate_fifo(fifos->fg_gcommand_r, "fg_gcommand_r");
	validate_fifo(fifos->gcommand_graphics_w, "gcommand_graphics_w");
	validate_fifo(fifos->gcommand_fg_w, "gcommand_fg_w");

	// Setup display
	if(render_init(1) != RENDER_OK) {
		printf("Failed to init renderer\n");
		return 0;
	}

	printf("Graphics command thread running\n");

	// Fill the background with black
	fillrect(0, 0, 1000, 1000, black);
	render_flip_buffer();

	unsigned int last_frame = get_microseconds();
	unsigned int last_render_time = 0;
	int frame_times[100] = {0};
	int frame_time_offset = 0;

	while(true) {
		unsigned int loop_start = get_microseconds();

		// If it is time to render another frame
		if ((get_microseconds() - last_frame) > 5000) {
			// Tell the graphics threads to draw
			fifos->gcommand_graphics_w->push(true);
			fifos->gcommand_fg_w->push(true);


			// Redraw the right side of the screen
			fillrect(500, 0, 640, 480, black);
			char text_buffer[32];
			sprintf(text_buffer, "Frame time (ms)%f", last_render_time / 1000.);
			drawstring(500, 10, text_buffer, red);

			// Wait for the drawing threads to draw their stuff
			fifos->graphics_gcommand_r->pop();
			fifos->fg_gcommand_r->pop();

			render_flip_buffer();
			last_frame = get_microseconds();

			last_render_time = last_frame - loop_start;
			frame_times[frame_time_offset] = last_render_time / 1000.;
			frame_time_offset = (frame_time_offset + 1) % 100;
		}

	}
}
