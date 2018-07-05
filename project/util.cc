#include "util.h"

#include <helix.h>

unsigned int get_microseconds() {
	unsigned int val = get_clock();

	int seconds = val >> 20;
	int microseconds = val & 0xfffff;

	return seconds * 1000000 + microseconds;
}
