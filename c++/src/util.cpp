#include "util.h"
#include <cstdlib>

int Util::randInt(const int max) {
	return rand() % max + 1;
}