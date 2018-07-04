#include "fourier_draw_thread.h"

#include "data.h"
#include "cores.h"

#include "graphics_util.h"

#include "fourier_params.h"

void* fourier_draw_thread(void* args) {
	Fifos* fifos = (Fifos*) args;

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

		// TODO: Flush cache
		// If it is time to render another frame
		if (!fifos->gcommand_fg_r->empty()) {
			printf("fourier: Starting draw\n");
			fifos->gcommand_fg_r->pop();
			fillrect(0, 200, 640, 500, black);

			draw_values(fourier_reals, 0, FOURIER_SIZE, 450, 3);

			printf("fourier: Telling graphics command that drawing is done\n");
			fifos->fg_gcommand_w->push(true);
			printf("fourier: Told graphics command that drawing is done\n");
		}
	}
}
