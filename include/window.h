
#include <stdlib.h>
#include <err.h>
#include <time.h>

#include "xdg-shell-client-protocol.h"
#include <wayland-egl.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "client_state.h"

void open_window(struct client_state *state, int32_t width, int32_t height, void (*draw)(struct client_state *state));
void close_window(struct client_state *state);
void render(struct client_state *state);

void set_fullscreen(struct client_state *state);
void unset_fullscreen(struct client_state *state);

void set_window_projection(struct client_state *state);
