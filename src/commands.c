#include "commands.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

void
c_quit(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	state->closed = true;

	state->dirty = true;

	return;
}

void
c_set_scale_mode(
	void *data,
	union argument scale_mode
) {
	struct client_state *state = data;

	state->scale_mode = scale_mode.i;

	state->dirty = true;

	return;
}


void
c_toggle_fullscreen(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	if (state->fullscreen) {
		unset_fullscreen(state);

	} else {
		set_fullscreen(state);
	}


	state->dirty = true;

	return;
}

void
c_toggle_bar(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	state->bar.hidden = !state->bar.hidden;

	state->dirty = true;

	return;
}

void
c_reload_image(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	unload_texture_without_shader(state);
	try_to_load_texture(state, state->fileinfo_index);

	state->dirty = true;

	return;
}

void
c_remove_image(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	unload_texture_without_shader(state);
	remove_fileinfo(state, state->fileinfo_index);
	try_to_load_texture(state, state->fileinfo_index);

	state->dirty = true;

	return;
}

void
c_first_image(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	unload_texture_without_shader(state);
	try_to_load_texture(state, 0);

	state->dirty = true;

	return;
}

void
c_n_or_last_image(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	unload_texture_without_shader(state);
	try_to_load_texture(state, state->prefix);

	state->dirty = true;

	return;
}

void
c_scroll(
	void *data,
	union argument direction
) {
	struct client_state *state = data;

	move_texture(state, direction.i);

	state->dirty = true;

	return;
}

void
c_scroll_to_edge(
	void *data,
	union argument direction
) {
	struct client_state *state = data;

	move_texture_to_edge(state, direction.i);

	state->dirty = true;

	return;
}

void
c_zoom(
	void *data,
	union argument direction
) {
	struct client_state *state = data;

	if (direction.i > 0) {
		zoom_in(state);

	} else if (direction.i < 0) {
		zoom_out(state);
	}

	state->dirty = true;

	return;
}

void
c_set_zoom(
	void *data,
	union argument zoom
) {
	struct client_state *state = data;

	state->zoom = zoom.f;

	state->dirty = true;

	return;
}

void
c_toggle_image_mark(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	toggle_fileinfo_mark(state, state->fileinfo_index);

	state->last_marked = state->fileinfo_index;

	state->dirty = true;

	return;
}

void
c_reverse_marks(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	for (size_t i = 0; i < state->amount_of_fileinfos; i++)
		toggle_fileinfo_mark(state, i);

	state->last_marked = state->amount_of_fileinfos - 1;

	state->dirty = true;

	return;
}

void
c_unmark_all(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	for (size_t i = 0; i < state->amount_of_fileinfos; i++)
		state->fileinfos[i].mark = false;

	state->last_marked = state->amount_of_fileinfos - 1;

	state->dirty = true;

	return;
}

void
c_mark_range(
	void *data,
	union argument _
) {
	struct client_state *state = data;

	int start = MIN(state->fileinfo_index, state->last_marked);
	int end = MAX(state->fileinfo_index, state->last_marked) + 1;

	bool mark = state->fileinfos[state->last_marked].mark;

	for (int i = start; i < end; i++)
		state->fileinfos[state->last_marked].mark = mark;

	state->last_marked = state->fileinfo_index;

	state->dirty = true;

	return;
}

void
c_navigate_marked(
	void *data,
	union argument direction
) {
	struct client_state *state = data;

	int delta;

	if (direction.i > 0) {
		delta = +1;
	} else {
		delta = -1;
	}

	int32_t end = (delta > 0) ? state->amount_of_fileinfos : -1;

	int32_t i;
	for (i = state->fileinfo_index + delta; i != end; i += delta) {
		if (state->fileinfos[i].mark)
			break;
	}

	if (i < 0 || state->amount_of_fileinfos <= i)
		return;

	unload_texture_without_shader(state);
	try_to_load_texture(state, i);

	state->dirty = true;

	return;
}

void
c_navigate(
	void *data,
	union argument delta
) {
	struct client_state *state = data;

	unload_texture_without_shader(state);
	try_to_load_texture(state, MAX((int32_t)state->fileinfo_index + delta.i, 0));

	state->dirty = true;

	return;
}

void
c_toggle_antialias(
	void *data,
	union argument direction
) {
	struct client_state *state = data;

	if (state->antialias) {
		unset_antialias(state);

	} else {
		set_antialias(state);
	}

	state->dirty = true;

	return;
}

void
c_change_gamma(
	void *data,
	union argument delta
) {
	struct client_state *state = data;

	return;
}
