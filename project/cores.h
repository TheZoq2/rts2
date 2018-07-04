#pragma once
#ifndef H_CORES
#define H_CORES

#include <helix.h>

#include "data.h"
// #include "types.h"

const int READ_CORE = 1;
const int GRAPHICS_CORE = 2;
const int FOURIER_CORE = 3;
const int TRIGGER_CORE = 4;
const int FOURIER_GRAPHICS_CORE = 5;
const int GRAPHICS_COMMAND_CORE = 6;
const int CORE_AMOUNT = 7;

struct Fifos {
	CFifoPtr<int16_t> read_trigger;
	CFifo<int16_t, CFifo<>::r>* read_trigger_r;
	CFifo<int16_t, CFifo<>::w>* read_trigger_w;

	CFifoPtr<int16_t> trigger_graphics;
	CFifo<int16_t, CFifo<>::r>* trigger_graphics_r;
	CFifo<int16_t, CFifo<>::w>* trigger_graphics_w;

	CFifoPtr<int16_t> trigger_fourier;
	CFifo<int16_t, CFifo<>::r>* trigger_fourier_r;
	CFifo<int16_t, CFifo<>::w>* trigger_fourier_w;

	CFifoPtr<std::pair<int16_t, float> > fourier_fg;
	CFifo<std::pair<int16_t, float>, CFifo<>::r>* fourier_fg_r;
	CFifo<std::pair<int16_t, float>, CFifo<>::w>* fourier_fg_w;

	CFifoPtr<bool> graphics_gcommand;
	CFifo<bool, CFifo<>::r>* graphics_gcommand_r;
	CFifo<bool, CFifo<>::w>* graphics_gcommand_w;
	CFifoPtr<bool> gcommand_graphics;
	CFifo<bool, CFifo<>::r>* gcommand_graphics_r;
	CFifo<bool, CFifo<>::w>* gcommand_graphics_w;

	CFifoPtr<bool> fg_gcommand;
	CFifo<bool, CFifo<>::r>* fg_gcommand_r;
	CFifo<bool, CFifo<>::w>* fg_gcommand_w;
	CFifoPtr<bool> gcommand_fg;
	CFifo<bool, CFifo<>::r>* gcommand_fg_r;
	CFifo<bool, CFifo<>::w>* gcommand_fg_w;

	bool valid() {
		if(!read_trigger.valid()) {
			printf("read->trigger fifo was invalid\n");
			return false;
		}
		if(!trigger_graphics.valid()) {
			printf("trigger->graphics fifo was invalid\n");
			return false;
		}
		if(!trigger_fourier.valid()) {
			printf("trigger->fourier fifo was invalid\n");
			return false;
		}
		if(!fourier_fg.valid()) {
			printf("fourier->foureier_graphics fifo was invalid\n");
			return false;
		}
		if(!graphics_gcommand.valid()) {
			printf("graphics->graphics_command fifo was invalid\n");
			return false;
		}
		if(!gcommand_graphics.valid()) {
			printf("gcommand->graphics fifo was invalid\n");
			return false;
		}
		if(!fg_gcommand.valid()) {
			printf("fourier_graphics->graphics_command fifo was invalid\n");
			return false;
		}
		if(!gcommand_fg.valid()) {
			printf("graphics_command->fourier graphics fifo was invalid\n");
			return false;
		}

		return true;
	}
};

#endif
