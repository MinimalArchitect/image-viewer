#ifndef MAPS_H
#define MAPS_H

#include "stdint.h"

union argument {
	int32_t i;
	float f;
};

struct key_map {
	uint32_t mask;
	uint32_t symbol;
	void (*function)(void *data, union argument argument);
	union argument argument;
};

#endif /* MAPS_H */
