// Rendering library demo
#include <helix.h>
#include <render/render.h>

const BALL_AMOUNT = 25;
const THREAD_AMOUNT = 31;

struct Ball {
	float x;
	float y;
	float vx;
	float vy;
};

struct ThreadParameters {
	ThreadParameters(CFifo<Ball, CFifo<>::w>* ballWr, int numBalls) {
		this->ballWr = ballWr;
		this->numBalls = numBalls;
	}
	CFifo<Ball, CFifo<>::w>* ballWr;
	int numBalls;
};


void* core(void* args) {
	ThreadParams* params = args;

	balls = [length]
	while() {
	}
}

CFifo<Ball, CFifo<>::w>* wr[BALL_AMOUNT];
CFifo<Ball, CFifo<>::r>* rd[BALL_AMOUNT];

int main(int argc, char **argv) {
	printf("Starting drawing demo\n");
	if(render_init(1) != RENDER_OK) {
		printf("Error: init display!\n");
		return 0;
	}
	
	for(int i = 0; i < THREAD_AMOUNT; i++) {
		pid_t pid;

		int numBalls = BALL_AMOUNT / THREAD_AMOUNT;
		if(i < BALL_AMOUNT % THREAD_AMOUNT) {
			numBalls += 1;
		}

		ThreadParameters args = ThreadParameters(wr[i], numBalls);
		if(int e=CreateProcess(pid, core, args, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, 1))
			ERREXIT2("Process creation failed: %i", e);
	}
	// Fill only a part of the screen
	// fillrect(0,0,319,479,black);
	// drawrect(50,50,200,90,red);
	// drawstring(70,70,"Render demo\r\nfor ML-605", yellow);
	render_flip_buffer();
	printf("Done\n");
}

