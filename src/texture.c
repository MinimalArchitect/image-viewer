#include "texture.h"

#include "config.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define CLAMP(value, min, max) MIN(MAX((value), (min)), (max))

static const char *vertex_shader =
	"#version 330 core						\n"
	"layout (location = 0) in vec4 vertex;				\n"
	"								\n"
	"out vec2 texture_coordinates;					\n"
	"								\n"
	"uniform mat4 projection;					\n"
	"								\n"
	"void main(void) {						\n"
	"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);	\n"
	"	texture_coordinates = vertex.zw;			\n"
	"}								\n";

static const char *fragment_shader =
	"#version 330 core						\n"
	"in vec2 texture_coordinates;					\n"
	"								\n"
	"out vec4 color;						\n"
	"								\n"
	"uniform sampler2D image;					\n"
	"								\n"
	"void main(void) {						\n"
	"	color =  texture(image, texture_coordinates);		\n"
	"}								\n";

static int32_t
load_image(
	struct image *image,
	const char *filename
) {
	FILE *file = fopen(filename, "rb");

	if (read_image(image, file) != 0)
		return -1;

	fclose(file);

	// TODO(lukas): uncompress image

	// TODO(lukas): learn to load more image formats

	if (strcmp(image->compression_algorithm, "uncompressed") != 0) {
		warnx("this compression is not implemented");
		destroy_image(image);

		return -1;
	}

	if (image->amount_of_channels != 3) {
		warnx("this amount of channels is not implemented");
		destroy_image(image);

		return -1;
	}

	if (strcmp(image->channel_info[0].name, "red") != 0
	 && strcmp(image->channel_info[1].name, "green") != 0
	 && strcmp(image->channel_info[2].name, "blue") != 0) {
		warnx("this channel configuration is not implemented");
		destroy_image(image);

		return -1;
	}
	return 0;
}

static int32_t
load_texture(
	struct texture *texture,
	const char *filename
) {
	struct image image = { 0 };

	if (load_image(&image, filename) != 0)
		return -1;

	texture->width = image.width;
	texture->height = image.height;

	// NOTE(lukas): convert image to gl_texture
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	destroy_image(&image);

	return 0;
}

void
set_antialias(
	struct client_state *state
) {
	state->antialias = true;

	glBindTexture(GL_TEXTURE_2D, state->texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return;
}

void
unset_antialias(
	struct client_state *state
) {
	state->antialias = false;

	glBindTexture(GL_TEXTURE_2D, state->texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	return;
}

void
try_to_load_texture(
	struct client_state *state,
	uint32_t index
) {
	index = MIN(index, state->amount_of_fileinfos - 1);

	if (state->amount_of_fileinfos == 0) {
		state->fileinfo_index = 0;

		if (!state->quiet)
			warnx("no more images to display");

		state->closed = true;

		return;
	}

	while (load_texture(&state->texture, state->fileinfos[index].name) != 0) {
		if (!state->quiet)
			warnx("could not read image: %s", state->fileinfos[index].name);

		remove_fileinfo(state, index);

		index = MIN(index, state->amount_of_fileinfos - 1);

		if (state->amount_of_fileinfos == 0)
			break;
	}

	if (state->amount_of_fileinfos == 0) {
		state->fileinfo_index = 0;

		if (!state->quiet)
			warnx("no more images to display");

		state->closed = true;

		return;
	}

	state->fileinfo_index = index;

	if (state->texture.shader.id == 0)
		load_shader(&state->texture.shader, vertex_shader, fragment_shader, NULL);

	if (state->antialias) {
		set_antialias(state);

	} else {
		unset_antialias(state);
	}

	state->dirty = true;

	return;
}

void
unload_texture_without_shader(
	struct client_state *state
) {
	glDeleteTextures(1, &state->texture.id);

	state->texture.id = 0;
	state->texture.width = 0;
	state->texture.height = 0;

	return;
}

void
unload_texture(
	struct client_state *state
) {
	glDeleteTextures(1, &state->texture.id);
	unload_shader(&state->texture.shader);

	state->texture.id = 0;
	state->texture.width = 0;
	state->texture.height = 0;

	return;
}

static void
draw_texture_area(
	struct texture *texture,
	struct camera *camera,
	float x,
	float y,
	float width,
	float height
) {
	vec3 bottom_left_corner_relative_to_camera_position;
	bottom_left_corner_relative_to_camera_position[0] = -(float)camera->x;
	bottom_left_corner_relative_to_camera_position[1] = -(float)camera->y;
	bottom_left_corner_relative_to_camera_position[2] = 0.0f;

	mat4 view;
	glm_ortho(-(float)camera->width / 2.0f, (float)camera->width / 2.0f, -(float)camera->height / 2.0f, (float)camera->height / 2.0f, 0.0f, -1.0f, view);

	vec3 bottom_left_corner;
	glm_mat4_mulv3(view, bottom_left_corner_relative_to_camera_position, 1.0f, bottom_left_corner);

	// NOTE(lukas): denormalize the coordinate
	float xpos = bottom_left_corner[0] * width / 2.0 + width / 2.0f + x;
	float ypos = bottom_left_corner[1] * height / 2.0 + height / 2.0f + y;

	float zoom = width / camera->width;
	float w = (float)texture->width * zoom;
	float h = (float)texture->height * zoom;

	set_shader(&texture->shader);

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

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	float vertices[6][4] = {
		{ xpos,     ypos + h, 0.0f, 0.0f },
		{ xpos,     ypos,     0.0f, 1.0f },
		{ xpos + w, ypos,     1.0f, 1.0f },

		{ xpos,     ypos + h, 0.0f, 0.0f },
		{ xpos + w, ypos,     1.0f, 1.0f },
		{ xpos + w, ypos + h, 1.0f, 0.0f }
	};

	// render image texture over quad
	glBindTexture(GL_TEXTURE_2D, texture->id);
	// update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	unset_shader(&texture->shader);

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
draw_texture(
	struct client_state *state
) {
	int32_t bar_offset = !state->bar.hidden ? state->bar.text_height : 0;
	state->camera = get_camera_with_scale_mode(state->scale_mode, state->texture.width, state->texture.height, state->width, state->height - bar_offset, state->camera.x, state->camera.y, state->zoom);

	set_shader_projection(&state->texture.shader, state->width, state->height);
	draw_texture_area(&state->texture, &state->camera, 0, bar_offset, state->width, state->height - bar_offset);

	return;
}

void
move_texture(
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
		if (!state->quiet)
			warnx("this direction is not implemented");
	}

	return;
}

void
move_texture_to_edge(
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
		if (!state->quiet)
			warnx("this direction is not implemented");
	}

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

	return;
}
