#ifndef BOWL_H
#define BOWL_H

#include "map.h"

struct Bowl {
	// generate mesh for current position.
	int width;
	int length;
	int *elevation;
};

struct Bowl *generateBowl(enum MapType type, int frame);
void freeBowl(struct Bowl *bowl);

#endif
