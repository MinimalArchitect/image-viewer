#include "display.h"

#include "config.h"

static void
noop() {}

static void
wl_keyboard_keymap(
        void *data,
        struct wl_keyboard *keyboard,
        uint32_t format,
        int fd,
        uint32_t size
) {
        struct client_state *state = data;

        assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

        char *map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
        assert(map_shm != MAP_FAILED);

        struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(state->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        munmap(map_shm, size);
        close(fd);

        struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);
        xkb_keymap_unref(state->xkb_keymap);
        xkb_state_unref(state->xkb_state);
        state->xkb_keymap = xkb_keymap;
        state->xkb_state = xkb_state;

        return;
}

static void
wl_keyboard_key(
	void *data,
	struct wl_keyboard *keyboard,
	uint32_t serial,
	uint32_t time,
	uint32_t key,
	uint32_t key_state
) {
	struct client_state *state = data;

	uint32_t keycode = key + 8;
	xkb_keysym_t symbol = xkb_state_key_get_one_sym(state->xkb_state, keycode);

	xkb_mod_mask_t mask = xkb_state_serialize_mods(state->xkb_state, XKB_STATE_MODS_EFFECTIVE);

	if (key_state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		for (int32_t i = 0;i < sizeof(key_maps) / sizeof(key_maps[0]); i++) {
			if (symbol == key_maps[i].symbol && mask == key_maps[i].mask)
				key_maps[i].function(state, key_maps[i].argument);
		}
	}

	return;
}

static void
wl_keyboard_modifiers(
	void *data,
	struct wl_keyboard *keyboard,
	uint32_t serial,
	uint32_t mods_depressed,
	uint32_t mods_latched,
	uint32_t mods_locked,
	uint32_t group
) {
	struct client_state *state = data;

	xkb_state_update_mask(state->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);

	return;
}

static const struct wl_keyboard_listener
keyboard_listener = {
	.keymap = wl_keyboard_keymap,
	.enter = noop,
	.leave = noop,
	.key = wl_keyboard_key,
	.modifiers = wl_keyboard_modifiers,
	.repeat_info = noop,
};

static void
pointer_handle_button(
	void *data,
	struct wl_pointer *pointer,
	uint32_t serial,
	uint32_t time,
	uint32_t button,
	uint32_t button_state
) {
	struct client_state *state = data;

	if (button == BTN_LEFT && button_state == WL_POINTER_BUTTON_STATE_PRESSED) {
		xdg_toplevel_move(state->xdg_toplevel, state->seat, serial);
	}

	return;
}

static const struct wl_pointer_listener
pointer_listener = {
	.enter = noop,
	.leave = noop,
	.motion = noop,
	.button = pointer_handle_button,
	.axis = noop,
	.frame = noop,
	.axis_source = noop,
	.axis_stop = noop,
	.axis_discrete = noop,
};

static void
seat_handle_capabilities(
	void *data,
	struct wl_seat *seat,
	uint32_t capabilities
) {
	struct client_state *state = data;

	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(keyboard, &keyboard_listener, state);
	}

	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, state);
	}

	return;
}

static const struct wl_seat_listener
seat_listener = {
	.capabilities = seat_handle_capabilities,
};

static void
xdg_wm_base_ping(
	void *data,
	struct xdg_wm_base *xdg_wm_base,
	uint32_t serial
) {
	struct client_state *state = data;

	xdg_wm_base_pong(xdg_wm_base, serial);

	return;
}

static const struct xdg_wm_base_listener
xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

static void
handle_global(
	void *data,
	struct wl_registry *registry,
	uint32_t name,
	const char *interface,
	uint32_t version
) {
	struct client_state *state = data;

	if (strcmp(interface, wl_seat_interface.name) == 0) {
		state->seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
		wl_seat_add_listener(state->seat, &seat_listener, state);

	} else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		state->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);

	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		state->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
	}

	return;
}

static void
handle_global_remove(
	void *data,
	struct wl_registry *registry,
	uint32_t name
) {
	struct client_state *state = data;

	return;
}

static const struct wl_registry_listener
registry_listener = {
	.global = handle_global,
	.global_remove = handle_global_remove,
};

void
connect_display(
	struct client_state *state
) {
	state->display = wl_display_connect(NULL);
	if (state->display == NULL)
		err(EXIT_FAILURE, "failed to create display");

	struct wl_registry *registry = wl_display_get_registry(state->display);
	state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	wl_registry_add_listener(registry, &registry_listener, state);

	wl_display_dispatch(state->display);
	wl_display_roundtrip(state->display);

	if (state->compositor == NULL || state->xdg_wm_base == NULL)
		err(EXIT_FAILURE, "no wl_shm, wl_compositor or xdg_wm_base support");

	state->egl_display = eglGetDisplay((EGLNativeDisplayType)state->display);
	if (state->egl_display == EGL_NO_DISPLAY)
		err(EXIT_FAILURE, "failed to create EGL display");

	EGLint major, minor;
	if (!eglInitialize(state->egl_display, &major, &minor))
		err(EXIT_FAILURE, "failed to initialize EGL");

	eglBindAPI(EGL_OPENGL_API);

	EGLint count;
	eglGetConfigs(state->egl_display, NULL, 0, &count);

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_NONE,
	};

	EGLint n = 0;
	EGLConfig *configs = calloc(count, sizeof(EGLConfig));
	eglChooseConfig(state->egl_display, config_attribs, configs, count, &n);
	if (n == 0)
		err(EXIT_FAILURE, "failed to choose an EGL config\n");

	state->egl_config = configs[0];

	EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE,
	};

	state->egl_context = eglCreateContext(state->egl_display, state->egl_config, EGL_NO_CONTEXT, context_attribs);

	return;
}

void
disconnect_display(
	struct client_state *state
) {
	eglDestroyContext(state->egl_display, state->egl_context);
	eglTerminate(state->egl_display);
	wl_display_disconnect(state->display);

	return;
}
