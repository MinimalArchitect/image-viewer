#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#include <GLES3/gl3.h>
#include "shader.h"
#include "image.h"

#include <cglm/cglm.h>

enum direction {
	DIRECTION_LEFT,
	DIRECTION_UP,
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
};

struct texture {
	GLuint id;
	struct shader shader;
	uint32_t width;
	uint32_t height;
};

#include "client_state.h"

struct client_state;

void try_to_load_texture(struct client_state *state, uint32_t index);
void unload_texture_without_shader(struct client_state *state);
void unload_texture(struct client_state *state);
void draw_texture(struct client_state *state);

void set_antialias(struct client_state *state);
void unset_antialias(struct client_state *state);

void move_texture(struct client_state *state, enum direction);
void move_texture_to_edge(struct client_state *state, enum direction);

void zoom_in(struct client_state *state);
void zoom_out(struct client_state *state);
void set_zoom(struct client_state *state);

#endif /* TEXTURE_H */
