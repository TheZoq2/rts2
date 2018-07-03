#include "trigger_thread.h"

#include "cores.h"
#include "data.h"

const int CHUNK_SIZE = 500;

void* trigger_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	bool was_high = false;
	int samples_sent = CHUNK_SIZE;
	bool old_value = 0;

	while (true) {
		// Read a value from the fifo
		int value = fifos->read_trigger_r->front();
		fifos->read_trigger_r->pop();

		// If we have already triggered and not sent a full frame to the display
		if(samples_sent < CHUNK_SIZE) {
			fifos->trigger_graphics_w->push(value);
			fifos->trigger_fourier_w->push(value);
			samples_sent++;
		}
		// Check if this was a trigger
		else if (was_high && value < -25 || (!was_high) && value > 25 ) {
			samples_sent = 0;
		}

		if(value < 0) {was_high = false;} else {was_high = true;};
	}
}
