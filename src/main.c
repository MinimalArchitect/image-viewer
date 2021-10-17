#include "options.h"
#include "client_state.h"
#include <locale.h>
#include "display.h"
#include "window.h"
#include "fileinfo.h"
#include "bar.h"
#include "texture.h"

#include "config.h"

// TODO: add gamma capability to shader
// TODO: add rotation capability
// TODO: add flip capability
// TODO: add alpha capability
// TODO: set geometry of window
// TODO: add icon for window

void draw(
	struct client_state *state
) {
	set_window_projection(state);
	// set projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, (float)state->width, 0.0f, (float)state->height, 0.0f, -1.0f);
	glViewport(0, 0, state->width, state->height);
	glMatrixMode(0);

	draw_texture(state);
	draw_bar(state);

	glFlush();

	return;
}

int
main(
	int argc,
	char *argv[]
) {
	struct options options = get_options(argc, argv);
	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	struct client_state state = { 0 };

	state.quiet = options.quiet;
	state.scale_mode = options.scale_mode;
	state.zoom = options.zoom;
	state.camera.x = state.width / 2.0f;
	state.camera.y = state.height / 2.0f;

	if (setlocale(LC_CTYPE, "") == NULL && setlocale(LC_COLLATE, "") == NULL && !state.quiet)
		warnx("no locale support");

	if (options.read_stdin)
		add_fileinfos_from_file(&state, stdin);

	if (options.recursive_filename_search) {
		for (int32_t i = 0; i < argc; i++)
			add_fileinfos_recursively(&state, argv[i]);

	} else {
		for (int32_t i = 0; i < argc; i++)
			add_fileinfo(&state, argv[i]);
	}

	connect_display(&state);
	open_window(&state, default_window_width, default_window_height, draw);

	load_bar(&state, font_path, font_size, text_color, bar_color, options.bar_hidden);
	try_to_load_texture(&state, options.starting_file_number);

	if (options.fullscreen) {
		set_fullscreen(&state);

	} else {
		unset_fullscreen(&state);
	}

	render(&state);

	while (wl_display_dispatch(state.display) != -1 && !state.closed);

	unload_bar(&state);
	unload_texture(&state);

	close_window(&state);
	disconnect_display(&state);

	if (options.write_stdout)
		write_fileinfo_to_file_depending_on_mark(&state, stdout);

	free(state.fileinfos);

	exit(EXIT_SUCCESS);
}
