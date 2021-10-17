#ifndef CAMERA_H
#define CAMERA_H

#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>

enum scale_mode {
	SCALE_FIT,
	SCALE_WIDTH,
	SCALE_HEIGHT,
	SCALE_DOWN,
};

struct camera {
	int32_t x;
	int32_t y;

	uint32_t width;
	uint32_t height;
};

struct camera get_camera_with_scale_mode(enum scale_mode scale_mode, uint32_t texture_width, uint32_t texture_height, uint32_t width, uint32_t height, int32_t x, int32_t y, float zoom);

#endif /* CAMERA_H */
