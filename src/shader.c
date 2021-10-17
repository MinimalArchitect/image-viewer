#include "shader.h"

static void check_compile_errors(
	GLuint shader_id,
	const char *type
) {
	GLint success;
	GLchar infoLog[1024];
	if(strcmp(type, "PROGRAM") != 0) {
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
			fprintf(stderr, "SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
		}
	} else {
		glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader_id, 1024, NULL, infoLog);
			fprintf(stderr, "PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
		}
	}
}

void
load_shader(
	struct shader *shader,
	const char *vertex_shader_source,
	const char *fragment_shader_source,
	const char *geometry_shader_source
) {
	shader->id = glCreateProgram();
	if (shader->id == 0)
		errx(EXIT_FAILURE, "could not create shader program");

	uint32_t vertex_shader, fragment_shader, geometry_shader;

	if (vertex_shader_source != NULL) {
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		if (vertex_shader == 0)
			errx(EXIT_FAILURE, "could not create vertex shader");

		glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
		glCompileShader(vertex_shader);

		check_compile_errors(vertex_shader, "VERTEX_SHADER");

		glAttachShader(shader->id, vertex_shader);
	}

	if (fragment_shader_source != NULL) {
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		if (fragment_shader == 0)
			errx(EXIT_FAILURE, "could not create fragment shader");

		glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
		glCompileShader(fragment_shader);

		check_compile_errors(fragment_shader, "FRAGMENT_SHADER");

		glAttachShader(shader->id, fragment_shader);
	}

	if (geometry_shader_source != NULL) {
		geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
		if (geometry_shader == 0)
			errx(EXIT_FAILURE, "could not create geometry shader");

		glShaderSource(geometry_shader, 1, &geometry_shader_source, NULL);
		glCompileShader(geometry_shader);

		check_compile_errors(geometry_shader, "GEOMETRY_SHADER");

		glAttachShader(shader->id, geometry_shader);
	}

	glLinkProgram(shader->id);
	check_compile_errors(shader->id, "PROGRAM");
	
	if (vertex_shader != 0)
		glDeleteShader(vertex_shader);

	if (geometry_shader != 0)
		glDeleteShader(fragment_shader);

	if (geometry_shader != 0)
		glDeleteShader(geometry_shader);

	return;
}

void
unload_shader(
	struct shader *shader
) {
	glDeleteProgram(shader->id);

	return;
}

void
set_shader(
	struct shader *shader
) {
	glUseProgram(shader->id);

	return;
}

void
unset_shader(
	struct shader *shader
) {
	glUseProgram(0);

	return;
}
