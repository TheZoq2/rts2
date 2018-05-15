#include <helix.h>
#include <stdio.h>

extern void* _ddrbase;

pthread_barrier_t bar;
// An error-checking wrapper for the barrier.
#define phase()															\
	do{																	\
		int res=pthread_barrier_wait(&bar);								\
		switch(res){													\
		case 0:															\
		case PTHREAD_BARRIER_SERIAL_THREAD:								\
			break;														\
		default:														\
			printf("Error in barrier: %d, %s\n",res,strerror(res));		\
			abort();													\
		}																\
	}while(0);

// Worker 1 runs on core 0
void worker1(dvi_frame* frame){
	
	// The phase() command stalls execution of this process until
	// worker2() also hits the same phase() call. In this way,
	// the executions of the workers are kept synchronized.

	phase(); //1
	// Fill a white rectangle
	printf("Fill white...\n");
	for(int y=200;y<400;y++)
		for(int x=0;x<DVI_WIDTH;x++)
			frame->set(x,y,white);
	
	phase(); //2
	// Wait for the other core to read some pixels.
	sleep(1);
	
	phase(); //3
	// Overwrite rectangle with blue.
	printf("Fill blue...\n");
	for(int y=200;y<400;y++)
		for(int x=0;x<DVI_WIDTH;x++)
			frame->set(x,y,blue);
	
	sleep(5);
	phase(); //4
	// Wait for the other core to write some pixels.

	sleep(5);
	phase(); //5
	// Draw two horizontal lines.
	for(int x=0;x<DVI_WIDTH;x++){
		frame->set(x,260,red);
		frame->set(x,263,red);
	}
	
	printf("Worker 1 done\n");
	phase(); //6
	
	// Suspend the execution, but do not terminate the process.
	pause();
}

// Worker 2 runs on core 1
void worker2(dvi_frame* frame){
	phase(); //1
	// Wait until the white rectangle is drawn.

	phase(); //2
	printf("Read pixel...\n");
	for(int x=50;x<DVI_WIDTH;x+=43)
		// This will read the color of one specific pixel.
		// Reading from cached memory loads a full cache line that contains
		// the requested data.
		*(volatile dvi_pixel*)frame->get(x,250);

	phase(); //3
	// Wait until the blue rectangle is drawn.
	
	phase(); //4
	for(int x=50;x<DVI_WIDTH;x+=43)
		// Write one pixel with a different color.
		// What you would expect is that in one horizontal line, multiple
		// pink dots on a blue background can be seen. Apparently, this is
		// not the case. Why?
		frame->set(x,250,pink);
	printf("Write pixel...\n");

	phase(); //5
	// Also draw two horizontal lines. Both workers write the same line simultaneously, but with
	// a different color. You would expect that the process that is executed a bit later,
	// will overwite the full line with its color (either red or green). This is not the case.
	// Please, sit down and observe the animation. What is happening?
	for(int x=0;x<DVI_WIDTH;x++){
		frame->set(x,260,green);
		frame->set(x,263,green);
	}

	phase(); //6
	printf("Worker 2 done\n");

	pause();
}

int main(int argc,char** argv){
	// Force screen to start of DDR, which is cached.
	// This is unsafe, as we overwrite the heap without using malloc()...
	// Only for demonstration purposes!
	dvi_set_address(&_ddrbase);
	dvi_frame* frame=dvi_get_address();

	// initialize barrier
	VERIFY(pthread_barrier_init(&bar,NULL,2),0);

	// start two workers
	par(worker2,frame,NULL,GetProcID()+1);
	worker1(frame);

	return 0;
}

