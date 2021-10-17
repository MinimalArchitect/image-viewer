#include "window.h"

static void
noop() {}

static void
xdg_surface_handle_configure(
	void *data,
	struct xdg_surface *xdg_surface,
	uint32_t serial
) {
	struct client_state *state = data;

	xdg_surface_ack_configure(state->xdg_surface, serial);

	return;
}

static const struct xdg_surface_listener
xdg_surface_listener = {
	.configure = xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_configure(
	void *data,
	struct xdg_toplevel *xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array *states
) {
	struct client_state *state = data;

	if (width == 0 || height == 0) {
		// NOTE(lukas): compositor is deferring to us
		return;
	}

	state->width = width;
	state->height = height;

	wl_egl_window_resize(state->egl_window, width, height, 0, 0);

	state->dirty = true;

	return;
}

static void
xdg_toplevel_handle_close(
	void *data,
	struct xdg_toplevel *xdg_toplevel
) {
	struct client_state *state = data;

	state->closed = true;

	return;
}

static const struct xdg_toplevel_listener
xdg_toplevel_listener = {
	.configure = xdg_toplevel_handle_configure,
	.close = xdg_toplevel_handle_close,
};

void
open_window(
	struct client_state *state,
	int32_t width,
	int32_t height,
	void (*draw)(struct client_state *state)
) {
	state->width = width;
	state->height = height;

	state->draw = draw;

	state->surface = wl_compositor_create_surface(state->compositor);
	state->xdg_surface = xdg_wm_base_get_xdg_surface(state->xdg_wm_base, state->surface);
	state->xdg_toplevel = xdg_surface_get_toplevel(state->xdg_surface);

	xdg_surface_add_listener(state->xdg_surface, &xdg_surface_listener, state);
	xdg_toplevel_add_listener(state->xdg_toplevel, &xdg_toplevel_listener, state);

	state->egl_window = wl_egl_window_create(state->surface, width, height);
	state->egl_surface = eglCreateWindowSurface(state->egl_display, state->egl_config, (EGLNativeWindowType)state->egl_window, NULL);

	wl_surface_commit(state->surface);
	wl_display_roundtrip(state->display);

	if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface, state->egl_context))
		errx(EXIT_FAILURE, "eglMakeCurrent failed\n");

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return;
}

void
close_window(
	struct client_state *state
) {
	xdg_toplevel_destroy(state->xdg_toplevel);
	xdg_surface_destroy(state->xdg_surface);
	wl_surface_destroy(state->surface);

	return;
}

static void
frame_handle_done(
	void *data,
	struct wl_callback *callback,
	uint32_t time
) {
	struct client_state *state = data;

	wl_callback_destroy(callback);
	render(state);

	return;
}

static const struct wl_callback_listener
frame_listener = {
	.done = frame_handle_done,
};

void
render(
	struct client_state *state
) {
	// Update color
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	long ms = (ts.tv_sec - state->last_frame.tv_sec) * 1000 +
		(ts.tv_nsec - state->last_frame.tv_nsec) / 1000000;
	size_t inc = (state->dec + 1) % 3;
	state->color[inc] += ms / 2000.0f;
	state->color[state->dec] -= ms / 2000.0f;
	if (state->color[state->dec] < 0.0f) {
		state->color[inc] = 1.0f;
		state->color[state->dec] = 0.0f;
		state->dec = inc;
	}
	state->last_frame = ts;

	// And draw a new frame
	if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface, state->egl_context))
		errx(EXIT_FAILURE, "eglMakeCurrent failed\n");

	glClearColor(state->color[0], state->color[1], state->color[2], 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	state->draw(state);

	// By default, eglSwapBuffers blocks until we receive the next frame event.
	// This is undesirable since it makes it impossible to process other events
	// (such as input events) while waiting for the next frame event. Setting
	// the swap interval to zero and managing frame events manually prevents
	// this behavior.
	eglSwapInterval(state->egl_display, 0);

	// Register a frame callback to know when we need to draw the next frame
	struct wl_callback *callback = wl_surface_frame(state->surface);
	wl_callback_add_listener(callback, &frame_listener, state);

	// This will attach a new buffer and commit the surface
	if (!eglSwapBuffers(state->egl_display, state->egl_surface))
		errx(EXIT_FAILURE, "eglSwapBuffers failed");

	return;
}

void set_fullscreen(
	struct client_state *state
) {
	state->fullscreen = true;

	xdg_toplevel_set_fullscreen(state->xdg_toplevel, NULL);

	return;
}

void unset_fullscreen(
	struct client_state *state
) {
	state->fullscreen = false;

	xdg_toplevel_unset_fullscreen(state->xdg_toplevel);

	state->width = 640;
	state->height = 480;

	wl_egl_window_resize(state->egl_window, state->width, state->height, 0, 0);

	state->dirty = true;

	return;
}

void
set_window_projection(
	struct client_state *state
) {
	// set projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, (float)state->width, 0.0f, (float)state->height, 0.0f, -1.0f);
	glViewport(0, 0, state->width, state->height);
	glMatrixMode(0);

	return;
}
