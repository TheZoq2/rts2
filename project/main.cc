#include <helix.h>

#include "read_thread.h"
#include "graphics_thread.h"
#include "fourier_thread.h"
#include "processing_threads.h"
#include "data.h"
#include "cores.h"
#include "defines.h"


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

	// Create a matrix of fifos
	CFifoPtr<Data> fifos[CORE_AMOUNT][CORE_AMOUNT];
	// Matrix of fifo read handles for inter-core communication. [from][to]
	// CFifo<Data, CFifo<>::r>* read_handles[CORE_AMOUNT][CORE_AMOUNT];
	// CFifo<Data, CFifo<>::w>* write_handles[CORE_AMOUNT][CORE_AMOUNT];
	Fifos fifo_handles;


	// Fill the matrix with fifos
	for(int i = 1; i < CORE_AMOUNT; i++) {
		for(int j = 1; j < CORE_AMOUNT; j++) {
			// We don't want to create a fifo from a core to itself
			if(i == j) { 
				// fifos[i][j] = NULL;
				fifo_handles.r[i][j] = NULL;
				fifo_handles.w[i][j] = NULL;
			}
			else {
				fifos[i][j] = CFifo<Data>::Create(i, fifo_handles.w[i][j], j, fifo_handles.r[i][j], FIFO_SIZE);
				if(!fifos[i][j].valid()) {
					ERREXIT2("Failed to create fifo [%i][%i]", i, j);
				}
				mc_flush(fifo_handles.w[i][j]);
				mc_flush(fifo_handles.r[i][j]);
			}
		}
	}


	pid_t read_pid = create_thread(read_thread, &fifo_handles, READ_CORE);
	pid_t graphics_pid = create_thread(graphics_thread, &fifo_handles, GRAPHICS_CORE);
	pid_t fourier_pid = create_thread(fourier_thread, &fifo_handles, FOURIER_CORE);

	// FIFOs are destroyed when the pointers goes out of scope
	if(int e=WaitProcess(read_pid, NULL, READ_CORE)) ERREXIT2("Waiting on read thread % i@%i: %i\n", read_pid, 1, e);
	if(int e=WaitProcess(fourier_pid, NULL, FOURIER_CORE)) ERREXIT2("Waiting on pong % i@%i: %i\n", fourier_pid, 2, e);
	if(int e=WaitProcess(graphics_pid, NULL, GRAPHICS_CORE)) ERREXIT2("Waiting on pong % i@%i: %i\n", graphics_pid, 3, e);

	return 0;
}


