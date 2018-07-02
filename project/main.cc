#include <helix.h>

#include "read_thread.h"
#include "graphics_thread.h"
#include "fourier_thread.h"
#include "processing_threads.h"
#include "data.h"
#include "cores.h"

#define ERREXIT(str) {fprintf(stderr, "Error: " str "\n"); exit(1);}
#define ERREXIT2(str, ...) {fprintf(stderr, "Error: " str "\n", __VA_ARGS__); exit(1);}


const int FIFO_SIZE = 10;

pid_t create_thread(void* (*function)(void*), void* args, int core) {
	pid_t pid;
	if(int e=CreateProcess(pid, function, args, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, core))
		ERREXIT2("Process creation failed: %i", e);

	if(int e=SetProcessFlags(pid, PROC_FLAG_JOINABLE, core))
		ERREXIT2("While setting process flags: %i", e);

	if(int e=StartProcess(pid, core))
		 ERREXIT2("Could not start read thread: %i", e);

	return pid;
}


int main(int argc, char **argv) {
	printf("starting \n");

	
	CFifoPtr<Data> fifos[CORE_AMOUNT][CORE_AMOUNT];

	for(int i = 3; i < CORE_AMOUNT; i++) {
		for(int j = 3; j < CORE_AMOUNT; j++) {
			if(i == j) { 
				// fifos[i][j] = NULL;
				READ_HANDLES[i][j] = NULL;
				WRITE_HANDLES[i][j] = NULL;
			}
			else {
				fifos[i][j] = CFifo<Data>::Create(i, WRITE_HANDLES[i][j], j, READ_HANDLES[i][j], FIFO_SIZE);
				if(fifos[i][j].valid()) {
					ERREXIT2("Failed to create fifo [%i][%i]", i, j);
				}
				mc_flush(WRITE_HANDLES[i][j]);
				mc_flush(READ_HANDLES[i][j]);
			}
		}
	}


	pid_t read_pid = create_thread(read_thread, NULL, READ_CORE);
	pid_t graphics_pid = create_thread(graphics_thread, NULL, GRAPHICS_CORE);
	pid_t fourier_pid = create_thread(fourier_thread, NULL, FOURIER_CORE);

	// FIFOs are destroyed when the pointers goes out of scope
	if(int e=WaitProcess(read_pid, NULL, READ_CORE)) ERREXIT2("Waiting on read thread % i@%i: %i\n", read_pid, 1, e);
	if(int e=WaitProcess(fourier_pid, NULL, FOURIER_CORE)) ERREXIT2("Waiting on pong % i@%i: %i\n", fourier_pid, 2, e);
	if(int e=WaitProcess(graphics_pid, NULL, GRAPHICS_CORE)) ERREXIT2("Waiting on pong % i@%i: %i\n", graphics_pid, 3, e);

	return 0;
}


