#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

#include "util.h"
#include "client_state.h"

struct options {
	// ===file-list===
	bool read_stdin;
	bool write_stdout;
	bool recursive_filename_search;
	uint32_t starting_file_number;

	// ===image===
	enum scale_mode scale_mode;
	float zoom;
	float gamma;

	// ===window===
	bool fullscreen;
	bool bar_hidden;
	char *geometry; // {width}x{height}±{x}±{y}

	// ===miscelaneous===
	bool quiet;
};

void usage(void);
struct options get_options(int32_t argc, char **argv);
