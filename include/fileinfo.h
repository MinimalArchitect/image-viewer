#ifndef FILEINFO_H
#define FILEINFO_H

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>

#include <unistd.h>

#include "client_state.h"

struct fileinfo {
	char *name;
	bool mark;
};

struct client_state;

void add_fileinfo(struct client_state *state, const char *filename);
void add_fileinfos_from_file(struct client_state *state, FILE *stream);
void add_fileinfos_recursively(struct client_state *state, const char *filename);

void remove_fileinfo(struct client_state *state, uint32_t index);

void toggle_fileinfo_mark(struct client_state *state, uint32_t index);

void write_fileinfo_to_file_depending_on_mark(struct client_state *state, FILE *stream);

#endif /* FILEINFO_H */
