#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <EGL/egl.h>
#include <stdbool.h>

#include "fileinfo.h"
#include "camera.h"
#include "texture.h"
#include "bar.h"

struct client_state {
	// Display stuff
	struct wl_display *display;
	struct wl_compositor *compositor;
	struct xdg_wm_base *xdg_wm_base;
	struct wl_seat *seat;

	EGLDisplay egl_display;
	EGLConfig egl_config;
	EGLContext egl_context;

	struct xkb_context *xkb_context;
	struct xkb_keymap *xkb_keymap;
	struct xkb_state *xkb_state;

	// Window stuff
	struct wl_surface *surface;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *xdg_toplevel;

	EGLSurface egl_surface;
	struct wl_egl_window *egl_window;

	// State
	int32_t width;
	int32_t height;

	bool closed;

	struct timespec last_frame;
	float color[3];
	size_t dec;

	size_t amount_of_fileinfos;
	struct fileinfo *fileinfos;
	uint32_t fileinfo_index;

	uint32_t last_marked;
	uint32_t prefix;

	struct camera camera;
	struct texture texture;
	struct bar bar;

	bool dirty;

	enum scale_mode scale_mode;
	float zoom;
	float gamma;
	bool antialias;
	bool fullscreen;

	bool quiet;

	void (*draw)(struct client_state *state);
};

#endif /* CLIENT_STATE_H */
