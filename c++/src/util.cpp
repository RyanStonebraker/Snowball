#include "util.h"
#include <cstdlib>
#include <time.h>

int Util::randInt(const int max) {
	srand(time(NULL));
	return (rand() / RAND_MAX) % max;
}
