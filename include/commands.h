#ifndef COMMANDS_H
#define COMMANDS_H

#include "client_state.h"
#include "display.h"
#include "window.h"
#include "fileinfo.h"
#include "texture.h"
#include "bar.h"

#include <string.h>
#include <GLES3/gl3.h>

#include "maps.h"

void c_quit(void *data, union argument _);
void c_set_scale_mode(void *data, union argument scale_mode);
void c_toggle_fullscreen(void *data, union argument _);
void c_toggle_bar(void *data, union argument _);
void c_reload_image(void *data, union argument _);
void c_remove_image(void *data, union argument _);
void c_first_image(void *data, union argument _);
void c_n_or_last_image(void *data, union argument _);
void c_scroll(void *data, union argument direction);
void c_scroll_to_edge(void *data, union argument direction);
void c_zoom(void *data, union argument direction);
void c_set_zoom(void *data, union argument zoom);
void c_toggle_image_mark(void *data, union argument _);
void c_reverse_marks(void *data, union argument _);
void c_unmark_all(void *data, union argument _);
void c_mark_range(void *data, union argument _);
void c_navigate_marked(void *data, union argument direction);
void c_navigate(void *data, union argument delta);
void c_toggle_antialias(void *data, union argument direction);

/*
// TODO(lukas)
void c_change_gamma(void *data, union argument _);
void c_rotate(void *data, union argument _);
void c_flip(void *data, union argument _);
void c_toggle_alpha(void *data, union argument _);
*/

#endif /* COMMANDS_H */
