#include "graphics_thread.h"

#include <stdio.h>
#include <helix.h>
#include <render/render.h>

#include "cores.h"
#include "data.h"
#include "defines.h"

const int SELF = GRAPHICS_CORE;

void* graphics_thread(void* args) {
	Fifos* fifos = (Fifos*) args;
	printf("graphics thread started\n");

	// Setup display
	if(render_init(1) != RENDER_OK) {
		printf("Failed to init renderer");
		return 0;
	}

	while(true) {
		Data data = fifos->r[READ_CORE][SELF]->front();
		fifos->r[READ_CORE][SELF]->pop();

		printf("Got value %i\n", data.value);
	}
	return 0;
}
