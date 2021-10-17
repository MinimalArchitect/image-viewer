#include "font.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define CLAMP(value, min, max) MIN(MAX((value), (min)), (max))

int32_t
load_font(
	struct font *font,
	const char *font_path,
	uint32_t font_size
) {
	FT_Library ft_lib;
	if (FT_Init_FreeType(&ft_lib))
		return -1;

	FT_Face face;
	if (FT_New_Face(ft_lib, font_path, 0, &face)) {
		FT_Done_FreeType(ft_lib);

		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, font_size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	size_t characters_capacity = 256;
	font->amount_of_characters = 0;
	font->characters = malloc(characters_capacity * sizeof(struct character));
	if (font->characters == NULL) {
		font->amount_of_characters = 0;

		return -1;
	}

	font->offset_y = 0;

	//NOTE(lukas): load textures
	FT_UInt glyph_index;
	uint32_t charcode = FT_Get_First_Char(face, &glyph_index);
	while(glyph_index != 0) {
		// NOTE(lukas): grow memory
		if (font->amount_of_characters == characters_capacity) {
			characters_capacity *= 2;
			font->characters = realloc(font->characters, characters_capacity * sizeof(struct character));
			if (font->characters == NULL)
				err(EXIT_FAILURE, "could not load font");
		}

		// NOTE(lukas): load character
		if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) {
			warnx("failed to load glyph: %d", charcode);
			continue;
		}

		// NOTE(lukas): generate texture
		uint32_t texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// NOTE(lukas): set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// NOTE(lukas): store character for later use
		font->characters[font->amount_of_characters] = (struct character) {
			.id = charcode,
			.texture_id = texture_id,
			.width = face->glyph->bitmap.width,
			.height = face->glyph->bitmap.rows,
			.bearing_x = face->glyph->bitmap_left,
			.bearing_y = face->glyph->bitmap_top,
			.advance = face->glyph->advance.x
		};

		font->offset_y = MAX(font->offset_y, font->characters[font->amount_of_characters].bearing_y - (int)font_size / 2);

		font->amount_of_characters++;

		charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft_lib);

	// TODO(lukas): sort character array based on id

	// NOTE(lukas): release unused memory
	font->characters = realloc(font->characters, font->amount_of_characters * sizeof(struct character));
	if (font->characters == NULL)
		return -1;

	return 0;
}

void
unload_font(
	struct font *font
) {
	for (size_t c = 0;c < font->amount_of_characters; c++)
		glDeleteTextures(1, &font->characters[c].texture_id);

	free(font->characters);
	font->characters = NULL;
	font->amount_of_characters = 0;

	return;
}
