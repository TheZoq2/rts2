#include "graphics_thread.h"

#include <stdio.h>
#include <helix.h>
#include <render/render.h>

#include "cores.h"
#include "data.h"
#include "defines.h"

const int SELF = GRAPHICS_CORE;

data_buffer_size = 1000;

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

	int data_buffer[DATA_BUFFER_SIZE]

	while(true) {
		Data data = fifos->r[READ_CORE][SELF]->front();
		fifos->r[READ_CORE][SELF]->pop();

		printf("Got value %i\n", data.value);
	}
	return 0;
}
