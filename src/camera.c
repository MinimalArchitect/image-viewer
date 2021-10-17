#include "camera.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define CLAMP(value, min, max) MIN(MAX((value), (min)), (max))

struct camera
get_camera_that_fits_width(
	uint32_t texture_width,
	uint32_t texture_height,
	uint32_t width,
	uint32_t height,
	int32_t y
) {
	float zoom = (float)width / texture_width;

	struct camera camera;

	camera.width = width / zoom;
	camera.height = height / zoom;

	camera.x = texture_width / 2;

	if (camera.height > texture_height) {
		camera.y = texture_height / 2;
	} else {
		uint32_t half = camera.height / 2;
		camera.y = CLAMP(y, half, texture_height - half);
	}

	return camera;
}

struct camera
get_camera_that_fits_height(
	uint32_t texture_width,
	uint32_t texture_height,
	uint32_t width,
	uint32_t height,
	int32_t x
) {
	float zoom = (float)height / texture_height;

	struct camera camera;

	camera.width = width / zoom;
	camera.height = height / zoom;

	camera.y = texture_height / 2;

	if (camera.width > texture_width) {
		camera.x = texture_width / 2;
	} else {
		uint32_t half = camera.width / 2;
		camera.x = CLAMP(x, half, texture_width - half);
	}

	return camera;
}

struct camera
get_camera_that_fits_texture(
	unsigned int texture_width,
	unsigned int texture_height,
	unsigned int width,
	unsigned int height
) {
	float vertical_zoom = (float)height / texture_height;
	float horizontal_zoom = (float)width / texture_width;

	if (vertical_zoom < horizontal_zoom) {
		return get_camera_that_fits_height(texture_width, texture_height, width, height, texture_width / 2);
	} else {
		return get_camera_that_fits_width(texture_width, texture_height, width, height, texture_height / 2);
	}
}

struct camera
get_camera_with_zoom(
	uint32_t texture_width,
	uint32_t texture_height,
	uint32_t width,
	uint32_t height,
	int32_t x,
	int32_t y,
	float zoom
) {
	float internal_zoom = zoom / 100.0f;

	struct camera camera;

	camera.width = width / internal_zoom;
	camera.height = height / internal_zoom;

	if (camera.width > texture_width) {
		camera.x = texture_width / 2;
	} else {
		uint32_t half = camera.width / 2;
		camera.x = CLAMP(x, half, texture_width - half);
	}

	if (camera.height > texture_height) {
		camera.y = texture_height / 2;
	} else {
		float half = camera.height / 2;
		camera.y = CLAMP(y, half, texture_height - half);
	}

	return camera;
}

struct camera
get_camera_with_scale_mode(
	enum scale_mode scale_mode,
	uint32_t texture_width,
	uint32_t texture_height,
	uint32_t width,
	uint32_t height,
	int32_t x,
	int32_t y,
	float zoom
) {
	switch (scale_mode) {
	case SCALE_FIT:
		return get_camera_that_fits_texture(texture_width, texture_height, width, height);
	case SCALE_WIDTH:
		return get_camera_that_fits_width(texture_width, texture_height, width, height, y);
	case SCALE_HEIGHT:
		return get_camera_that_fits_height(texture_width, texture_height, width, height, x);
	case SCALE_DOWN:
		return get_camera_with_zoom(texture_width, texture_height, width, height, x, y, zoom);
	default:
		err(EXIT_FAILURE, "this scale mode is not implemented");
	}


}
/*
void
move_camera(
	struct client_state *state,
	enum direction direction
) {
	switch (direction) {
	case DIRECTION_LEFT:
		state->camera.x -= state->camera.width / pan_fraction;
		break;

	case DIRECTION_RIGHT:
		state->camera.x += state->camera.width / pan_fraction;
		break;

	case DIRECTION_UP:
		state->camera.y += state->camera.height / pan_fraction;
		break;

	case DIRECTION_DOWN:
		state->camera.y -= state->camera.height / pan_fraction;
		break;

	default:
		warnx("this direction is not implemented");
	}

	return;
}

void
move_camera_to_edge(
	struct client_state *state,
	enum direction direction
) {
	switch (direction) {
	case DIRECTION_LEFT:
		state->camera.x = state->camera.width / 2;
		break;

	case DIRECTION_RIGHT:
		state->camera.x = state->texture.width - state->camera.width / 2;
		break;

	case DIRECTION_UP:
		state->camera.y = state->texture.height - state->camera.height / 2;
		break;

	case DIRECTION_DOWN:
		state->camera.y = state->camera.width / 2;
		break;

	default:
		warnx("this direction is not implemented");
	}

	state->dirty = true;

	return;
}

void
zoom_in(
	struct client_state *state
) {
	int32_t i;
	for (i = 0; i < sizeof(zoom_levels) / sizeof(zoom_levels[0]) - 1; i++) {
		if (state->zoom < zoom_levels[i])
			break;
	}

	state->zoom = zoom_levels[i];

	state->dirty = true;

	return;
}

void
zoom_out(
	struct client_state *state
) {
	int32_t i;
	for (i = sizeof(zoom_levels) / sizeof(zoom_levels[0]) - 1; i > 0; i--) {
		if (zoom_levels[i] < state->zoom)
			break;
	}

	state->zoom = zoom_levels[i];

	state->dirty = true;

	return;
}
*/
