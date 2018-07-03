#include <helix.h>

#include "read_thread.h"
#include "graphics_thread.h"
#include "fourier_thread.h"
#include "processing_threads.h"
#include "data.h"
#include "cores.h"
#include "defines.h"


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


	// Matrix of fifo read handles for inter-core communication. [from][to]
	Fifos fifo_handles;

	fifo_handles.read_graphics =
		CFifo<int>::Create(READ_CORE, fifo_handles.read_graphics_w, GRAPHICS_CORE, fifo_handles.read_graphics_r, 32);
	fifo_handles.read_fourier =
		CFifo<int>::Create(READ_CORE, fifo_handles.read_fourier_w, FOURIER_CORE, fifo_handles.read_fourier_r, 32);
	fifo_handles.fourier_graphics =
		CFifo<float*>::Create(FOURIER_CORE, fifo_handles.fourier_graphics_w, GRAPHICS_CORE, fifo_handles.fourier_graphics_r, 2);

	if (!fifo_handles.valid()) {
		 ERREXIT2("Could not start read thread: %i", 0);
	}

	pid_t read_pid = create_thread(read_thread, &fifo_handles, READ_CORE);
	pid_t graphics_pid = create_thread(graphics_thread, &fifo_handles, GRAPHICS_CORE);
	pid_t fourier_pid = create_thread(fourier_thread, &fifo_handles, FOURIER_CORE);

	// FIFOs are destroyed when the pointers goes out of scope
	if(int e=WaitProcess(read_pid, NULL, READ_CORE)) {
		ERREXIT2("Waiting on read thread % i@%i: %i\n", read_pid, 1, e);
	}
	if(int e=WaitProcess(fourier_pid, NULL, FOURIER_CORE)) {
		ERREXIT2("Waiting on fourier thread % i@%i: %i\n", fourier_pid, 2, e);
	}
	if(int e=WaitProcess(graphics_pid, NULL, GRAPHICS_CORE)) {
		ERREXIT2("Waiting on graphics thread % i@%i: %i\n", graphics_pid, 3, e);
	}

	return 0;
}


