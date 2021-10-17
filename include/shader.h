#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <string.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>

struct shader {
	GLuint id;
};

void load_shader(struct shader *shader, const char *vertes_shader_source, const char *fragment_shader_source, const char *geometry_shader_source);
void unload_shader(struct shader *shader);

void set_shader(struct shader *shader);
void unset_shader(struct shader *shader);

#endif /* SHADER_H */
