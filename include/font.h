#ifndef FONT_H
#define FONT_H

#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>
#include <GLES3/gl3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct character {
	wchar_t id;
	GLuint texture_id;
	uint32_t width;
	uint32_t height;
	int32_t bearing_x;
	int32_t bearing_y;
	int32_t advance;
};

struct font {
	struct character *characters;
	size_t amount_of_characters;

	int32_t offset_y;
};

int32_t load_font(struct font *font, const char *font_path, uint32_t font_size);
void unload_font(struct font *font);

#endif /* FONT_H */
