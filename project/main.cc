#include <helix.h>

#include "read_thread.h"
#include "graphics_thread.h"
#include "fourier_thread.h"
#include "processing_threads.h"
#include "data.h"
#include "cores.h"
#include "defines.h"
#include "fourier_params.h"
#include "trigger_thread.h"
#include "graphics_command.h"
#include "fourier_draw_thread.h"


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


	Fifos fifo_handles;

	fifo_handles.read_trigger =
		CFifo<int16_t>::Create(READ_CORE, fifo_handles.read_trigger_w, TRIGGER_CORE, fifo_handles.read_trigger_r, 1);
	fifo_handles.trigger_graphics =
		CFifo<int16_t>::Create(TRIGGER_CORE, fifo_handles.trigger_graphics_w, GRAPHICS_CORE, fifo_handles.trigger_graphics_r, FOURIER_SIZE);
	fifo_handles.trigger_fourier =
		CFifo<int16_t>::Create(TRIGGER_CORE, fifo_handles.trigger_fourier_w, FOURIER_CORE, fifo_handles.trigger_fourier_r, 1);
	fifo_handles.fourier_fg =
		CFifo<std::pair<int16_t, float> >::Create(FOURIER_CORE, fifo_handles.fourier_fg_w, FOURIER_GRAPHICS_CORE, fifo_handles.fourier_fg_r, FOURIER_SIZE);
	fifo_handles.fg_gcommand =
		CFifo<bool>::Create(FOURIER_GRAPHICS_CORE, fifo_handles.fg_gcommand_w, GRAPHICS_COMMAND_CORE, fifo_handles.fg_gcommand_r, 1);
	fifo_handles.gcommand_fg =
		CFifo<bool>::Create(GRAPHICS_COMMAND_CORE, fifo_handles.gcommand_fg_w, FOURIER_GRAPHICS_CORE, fifo_handles.gcommand_fg_r, 1);
	fifo_handles.graphics_gcommand =
		CFifo<bool>::Create(GRAPHICS_CORE, fifo_handles.graphics_gcommand_w, GRAPHICS_COMMAND_CORE, fifo_handles.graphics_gcommand_r, 1);
	fifo_handles.gcommand_graphics =
		CFifo<bool>::Create(GRAPHICS_COMMAND_CORE, fifo_handles.gcommand_graphics_w, GRAPHICS_CORE, fifo_handles.gcommand_graphics_r, 1);

	if (!fifo_handles.valid()) {
		 ERREXIT2("A fifo handle was invalid\n", 0);
	}

	pid_t read_pid = create_thread(read_thread, &fifo_handles, READ_CORE);
	pid_t graphics_pid = create_thread(graphics_thread, &fifo_handles, GRAPHICS_CORE);
	pid_t fourier_pid = create_thread(fourier_thread, &fifo_handles, FOURIER_CORE);
	pid_t trigger_pid = create_thread(trigger_thread, &fifo_handles, TRIGGER_CORE);
	pid_t fg_pid = create_thread(fourier_draw_thread, &fifo_handles, FOURIER_GRAPHICS_CORE);
	pid_t gcommand_pid = create_thread(graphics_command_thread, &fifo_handles, GRAPHICS_COMMAND_CORE);

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
	if(int e=WaitProcess(trigger_pid, NULL, TRIGGER_CORE)) {
		ERREXIT2("Waiting on trigger thread % i@%i: %i\n", trigger_pid, 3, e);
	}
	if(int e=WaitProcess(fg_pid, NULL, FOURIER_GRAPHICS_CORE)) {
		ERREXIT2("Waiting on fourier_graphics thread % i@%i: %i\n", fg_pid, 3, e);
	}
	if(int e=WaitProcess(gcommand_pid, NULL, GRAPHICS_COMMAND_CORE)) {
		ERREXIT2("Waiting on graphics command thread % i@%i: %i\n", gcommand_pid, 3, e);
	}

	return 0;
}


