// Rendering library demo
#include <helix.h>
#include <render/render.h>

int main(int argc, char **argv) {
	printf("Starting drawing demo\n");
	if(render_init(1) != RENDER_OK) {
		printf("Error: init display!\n");
		return 0;
	}

	int iterations = 10;
	while(true) {
		printf("%i iterations\n", iterations);
		int color = 1;
		int squareSize = iterations;
		for(int x = 0; x < 100 * squareSize; x += squareSize) {
			color = 1 - color;
			for(int y = 0; y < 100 * squareSize; y += squareSize) {
				if(color){
					fillrect(x, y, x+squareSize, y+squareSize,white);
				} else {
					fillrect(x, y, x+squareSize, y+squareSize, black);
				}
				color = 1 - color;
			}
		}
		iterations++;
		printf("%i iterations\n", iterations);
		render_flip_buffer();
	}
	
	// Fill only a part of the screen
	// fillrect(0,0,319,479,black);
	// drawrect(50,50,200,90,red);
	// drawstring(70,70,"Render demo\r\nfor ML-605", yellow);
	printf("Done\n");
}

