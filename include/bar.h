#ifndef BAR_H
#define BAR_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>

#include <cglm/cglm.h>

#include <wchar.h>


#include "font.h"
#include "shader.h"

struct bar {
	struct font font;
	struct shader shader;

	float background_color[3];
	float foreground_color[3];

	uint32_t width;
	uint32_t text_height;

	bool hidden;
};

#include "client_state.h"

struct client_state;

void load_bar(struct client_state *state, const char *font_path, uint32_t font_size, uint32_t text_color, uint32_t background_color, bool bar_hidden);
void unload_bar(struct client_state *state);
void draw_bar(struct client_state *state);

#endif /* BAR_H */
