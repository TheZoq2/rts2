#include <helix.h>

CFifo<bool,CFifo<>::w> *wr1; // Write end point of a FIFO
CFifo<bool,CFifo<>::r> *rd1; // Read end point of a FIFO

CFifo<bool,CFifo<>::w> *wr2; // Write end point of a FIFO
CFifo<bool,CFifo<>::r> *rd2; // Read end point of a FIFO

CFifo<bool,CFifo<>::w> *wr3; // Write end point of a FIFO
CFifo<bool,CFifo<>::r> *rd3; // Read end point of a FIFO

CFifo<bool,CFifo<>::w> *wr4; // Write end point of a FIFO
CFifo<bool,CFifo<>::r> *rd4; // Read end point of a FIFO

#define ERREXIT(str) {fprintf(stderr, "Error: " str "\n"); exit(1);}
#define ERREXIT2(str, ...) {fprintf(stderr, "Error: " str "\n", __VA_ARGS__); exit(1);}

void *ping(void *arg) {
	wr1->validate();
	rd4->validate();

	wr1->push(true);
	while(1) {
		rd4->pop();
		printf("Ping\n");
		wr1->push(true);
	}
	return NULL;
}

void *pong(void *arg) {
	wr2->validate();
	rd1->validate();
	while(1) {
		rd1->pop();
		printf("\tPong\n");
		wr2->push(true);
	}
	return NULL;
}

void *peng(void *arg) {
	wr3->validate();
	rd2->validate();
	while(1) {
		rd2->pop();
		printf("\t\tPeng\n");
		wr3->push(true);
	}
	return NULL;
}
void *pung(void *arg) {
	wr4->validate();
	rd3->validate();
	while(1) {
		rd3->pop();
		printf("\t\t\tPung\n");
		wr4->push(true);
	}
	return NULL;
}

int main(int argc, char **argv) {
	printf("starting \n");

	pid_t pid0, pid1, pid2, pid3;
	CFifoPtr<bool> fifo1 = CFifo<bool>::Create(1, wr1, 2, rd1, 2);
	if(!fifo1.valid()) ERREXIT("Error creating buffer");

	CFifoPtr<bool> fifo2 = CFifo<bool>::Create(2, wr2, 3, rd2, 2);
	if(!fifo2.valid()) ERREXIT("Error creating buffer");

	CFifoPtr<bool> fifo3 = CFifo<bool>::Create(3, wr3, 4, rd3, 2);
	if(!fifo3.valid()) ERREXIT("Error creating buffer");

	CFifoPtr<bool> fifo4 = CFifo<bool>::Create(4, wr4, 1, rd4, 2);
	if(!fifo4.valid()) ERREXIT("Error creating buffer");

	// Explicitely flush the cache line containing the global variables containing the FIFOs
	mc_flush(wr1);
	mc_flush(rd1);
	mc_flush(wr2);
	mc_flush(rd2);
	mc_flush(wr3);
	mc_flush(rd3);
	mc_flush(wr4);
	mc_flush(rd4);

	printf("Starting processes\n");

	if(int e=CreateProcess(pid0, ping, NULL, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, 1))
		ERREXIT2("Process creation failed: %i", e);
	if(int e=CreateProcess(pid1, pong, NULL, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, 2))
		ERREXIT2("Process creation failed: %i", e);
	if(int e=CreateProcess(pid2, peng, NULL, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, 3))
		ERREXIT2("Process creation failed: %i", e);
	if(int e=CreateProcess(pid3, pung, NULL, PROC_DEFAULT_TIMESLICE, PROC_DEFAULT_STACK, 4))
		ERREXIT2("Process creation failed: %i", e);

	if(int e=SetProcessFlags(pid0, PROC_FLAG_JOINABLE, 1))
		ERREXIT2("While setting process flags: %i", e);
	if(int e=SetProcessFlags(pid1, PROC_FLAG_JOINABLE, 2))
		ERREXIT2("While setting process flags: %i", e);
	if(int e=SetProcessFlags(pid2, PROC_FLAG_JOINABLE, 3))
		ERREXIT2("While setting process flags: %i", e);
	if(int e=SetProcessFlags(pid3, PROC_FLAG_JOINABLE, 4))
		ERREXIT2("While setting process flags: %i", e);

	if(int e=StartProcess(pid0, 1)) ERREXIT2("Could not start ping: %i", e);
	if(int e=StartProcess(pid1, 2)) ERREXIT2("Could not start pong: %i", e);
	if(int e=StartProcess(pid2, 3)) ERREXIT2("Could not start peng: %i", e);
	if(int e=StartProcess(pid3, 4)) ERREXIT2("Could not start pung: %i", e);

	// FIFOs are destroyed when the pointers goes out of scope
	if(int e=WaitProcess(pid0, NULL, 1)) ERREXIT2("Waiting on ping % i@%i: %i\n", pid0, 1, e);
	if(int e=WaitProcess(pid1, NULL, 2)) ERREXIT2("Waiting on pong % i@%i: %i\n", pid1, 2, e);
	if(int e=WaitProcess(pid2, NULL, 3)) ERREXIT2("Waiting on pong % i@%i: %i\n", pid2, 3, e);
	if(int e=WaitProcess(pid3, NULL, 4)) ERREXIT2("Waiting on pong % i@%i: %i\n", pid3, 4, e);

	return 0;
}

