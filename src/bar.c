#include "bar.h"

#include "config.h"

static const char *vertex_shader =
	"#version 330 core									\n"
	"layout (location = 0) in vec4 vertex;							\n"
	"out vec2 texture_coordinates;								\n"
	"											\n"
	"uniform mat4 projection;								\n"
	"											\n"
	"void main(void) {									\n"
	"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);				\n"
	"	texture_coordinates = vertex.zw;						\n"
	"}											\n";

static const char *fragment_shader =
	"#version 330 core									\n"
	"in vec2 texture_coordinates;								\n"
	"out vec4 color;									\n"
	"											\n"
	"uniform sampler2D text;								\n"
	"uniform vec3 text_color;								\n"
	"											\n"
	"void main(void) {									\n"
	"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texture_coordinates).r);	\n"
	"	color = vec4(text_color, 1.0) * sampled;					\n"
	"}											\n";

void
load_bar(
	struct client_state *state,
	const char *font_path,
	uint32_t font_size,
	uint32_t text_color,
	uint32_t background_color,
	bool bar_hidden
) {
	if (load_font(&state->bar.font, font_path, font_size) != 0)
		errx(EXIT_FAILURE, "could not load font: %s", font_path);

	load_shader(&state->bar.shader, vertex_shader, fragment_shader, NULL);
	if (state->bar.shader.id == 0)
		err(EXIT_FAILURE, "could not load font shader");

	state->bar.background_color[0] = ((background_color >> 16) & 0xFF) / 256.0f;
	state->bar.background_color[1] = ((background_color >> 8) & 0xFF) / 256.0f;
	state->bar.background_color[2] = ((background_color >> 0) & 0xFF) / 256.0f;

	state->bar.foreground_color[0] = ((text_color >> 16) & 0xFF) / 256.0f;
	state->bar.foreground_color[1] = ((text_color >> 8) & 0xFF) / 256.0f;
	state->bar.foreground_color[2] = ((text_color >> 0) & 0xFF) / 256.0f;

	state->bar.width = state->width;
	state->bar.text_height = font_size;
	state->bar.hidden = bar_hidden;

	return;
}

void
unload_bar(
	struct client_state *state
) {

	state->bar.width = 0;
	state->bar.text_height = 0;
	state->bar.hidden = false;

	state->bar.background_color[0] = 0.0f;
	state->bar.background_color[1] = 0.0f;
	state->bar.background_color[2] = 0.0f;

	state->bar.foreground_color[0] = 0.0f;
	state->bar.foreground_color[1] = 0.0f;
	state->bar.foreground_color[2] = 0.0f;

	unload_shader(&state->bar.shader);
	unload_font(&state->bar.font);

	return;
}

void
draw_full_rectangle(
	struct shader *shader,
	float color[3],
	float x,
	float y,
	float width,
	float height
) {
	set_shader(shader);

	GLfloat rectangle[] = {
		x, y,
		x + width, y,
		x + width, y + height,
		x, y + height
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(color[0], color[1], color[2]);
	glVertexPointer(2, GL_FLOAT, 0, rectangle);
	glDrawArrays(GL_QUADS, 0, 4);

	unset_shader(shader);

	return;
}

static void
set_shader_projection(
	struct shader *shader,
	uint32_t width,
	uint32_t height
) {
	mat4 projection;
	glm_ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, -1.0f, projection);

	set_shader(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader->id, "projection"), 1, GL_FALSE, (GLfloat *)projection);

	unset_shader(shader);

	return;
}

void
draw_text(
	struct font *font,
	struct shader *shader,
	float color[3],
	uint32_t x,
	uint32_t y,
	float scale,
	const char* format,
	...
) {
	char *string;

	va_list variadic_list;
	va_start(variadic_list, format);

	if (vasprintf(&string, format, variadic_list) < 0)
		err(EXIT_FAILURE, "could not draw text");

	const size_t size = strlen(string);
	wchar_t *wstring = calloc(size + 1, sizeof(wchar_t));
	mbstowcs(wstring, string, size);

	uint32_t vao, vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	set_shader(shader);

	glUniform3f(glGetUniformLocation(shader->id, "text_color"), color[0], color[1], color[2]);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	for (size_t i = 0; i < wcslen(wstring); i++) {
		// TODO(lukas): change to binary search
		// NOTE(lukas): search character in glyph memory
		size_t c;
		for (c = 0;c < font->amount_of_characters;c++) {
			if (font->characters[c].id == wstring[i])
				break;
		}

		// NOTE(lukas): have not found character
		if (c == font->amount_of_characters)
			continue;

		struct character character = font->characters[c];

		float xpos = x + character.bearing_x * scale;
		float ypos = y - ((float)character.height - character.bearing_y) * scale;

		float w = character.width * scale;
		float h = character.height * scale;

		// NOTE(lukas): update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h, 0.0f, 0.0f },
			{ xpos,     ypos,     0.0f, 1.0f },
			{ xpos + w, ypos,     1.0f, 1.0f },

			{ xpos,     ypos + h, 0.0f, 0.0f },
			{ xpos + w, ypos,     1.0f, 1.0f },
			{ xpos + w, ypos + h, 1.0f, 0.0f }
		};

		struct shader empty_shader = { 0 };

		// NOTE(lukas): render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, character.texture_id);
		// NOTE(lukas): update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// NOTE(lukas): render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// NOTE(lukas): now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	unset_shader(shader);

	free(wstring);
	free(string);

	return;
}

void
draw_bar(
	struct client_state *state
) {
	struct shader empty_shader = { 0 };

	if (!state->bar.hidden) {
		draw_full_rectangle(&empty_shader, state->bar.background_color, 0, 0, state->bar.width, state->bar.text_height);

		uint32_t index = state->fileinfo_index;
		const char *mark_text = state->fileinfos[index].mark ? "(marked) " : "";

		state->bar.width = state->width;

		set_shader_projection(&state->bar.shader, state->width, state->height);

		draw_text(&state->bar.font, &state->bar.shader, state->bar.foreground_color, 0, state->bar.font.offset_y / 2, 1.0f, "[%d/%d] %s%s", index + 1, state->amount_of_fileinfos, mark_text, state->fileinfos[index].name);
//		draw_text_from_right(state->bar.font, state->bar.shader, state->bar.foreground_color, state->width, 0, 1.0f, "[%s] ");
	}

	return;
}
