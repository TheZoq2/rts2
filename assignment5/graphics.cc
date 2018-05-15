// Rendering library demo
#include <helix.h>
#include <render/render.h>

int main(int argc, char **argv) {
	printf("Starting drawing demo\n");
	if(render_init(1) != RENDER_OK) {
		printf("Error: init display!\n");
		return 0;
	}
	
	// Fill only a part of the screen
	fillrect(0,0,319,479,black);
	drawrect(50,50,200,90,red);
	drawstring(70,70,"Render demo\r\nfor ML-605", yellow);
	render_flip_buffer();
	printf("Done\n");
}

