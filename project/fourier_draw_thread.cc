#include "fourier_draw_thread.h"

#include "data.h"
#include "cores.h"
#include "util.h"

#include "graphics_util.h"

#include "fourier_params.h"

void* fourier_draw_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

	validate_fifo(fifos->fourier_fg_r, "fourier_fg_r");
	validate_fifo(fifos->gcommand_fg_r, "gcommand_fg_r");
	validate_fifo(fifos->fg_gcommand_w, "fg_gcommand_w");

	printf("Fourier drawing thread starting\n");

	int fourier_current_offset = 0;
	int fourier_reals[FOURIER_SIZE];

	while(true) {
		// Read data from the fourier thread
		if(!fifos->fourier_fg_r->empty()) {
			std::pair<int, float> data = fifos->fourier_fg_r->front();
			fifos->fourier_fg_r->pop();

			fourier_reals[data.first] = data.second;
		}

		// If it is time to render another frame
		if (!fifos->gcommand_fg_r->empty()) {
			fifos->gcommand_fg_r->pop();
			fillrect(0, 200, 640, 500, black);

			draw_values(fourier_reals, 0, FOURIER_SIZE, 450, 3);

			FlushDCache();

			fifos->fg_gcommand_w->push(true);
		}
	}
}
