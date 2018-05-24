#include <helix.h>

#define ERREXIT(str) {fprintf(stderr, "Error: " str "\n"); exit(1);}
#define ERREXIT2(str, ...) {fprintf(stderr, "Error: " str "\n", __VA_ARGS__); exit(1);}

int main(int argc, char **argv) {
	printf("starting \n");

	return 0;
}

