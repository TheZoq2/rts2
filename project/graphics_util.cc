#include "graphics_util.h"

void draw_values(int8_t* values, int start, int length, int y_offset, int x_step, int x_start) {
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

