static const int32_t default_window_width = 640;
static const int32_t default_window_height = 480;

static const float zoom_levels[] = { 12.5f, 25.0f, 50.0f, 75.0f, 100.0f, 150.0f, 200.0f, 400.0f, 800.0f };

static const char *font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
static const uint32_t font_size = 20;

static const uint32_t text_color = 0xd8dee9;
static const uint32_t bar_color = 0x2e3440;
static const uint32_t background_color = 0x40566a;

static const float gamma_max = 10.0f;
static const float gamma_range = 32.0f;

static const int32_t pan_fraction = 5;

static const bool anti_alias = true;
static const bool alpha_layer = false;

#include "maps.h"
#include "commands.h"
static const struct key_map key_maps[] = {
	// mask,	key_symbol,		function,		argument
	{ 0,		XKB_KEY_q,		c_quit,			{ .i = 0 } },
	{ 0,		XKB_KEY_w,		c_set_scale_mode,	{ .i = SCALE_DOWN } },
	{ ShiftMask,	XKB_KEY_W,		c_set_scale_mode,	{ .i = SCALE_FIT } },
	{ 0,		XKB_KEY_e,		c_set_scale_mode,	{ .i = SCALE_HEIGHT } },
	{ ShiftMask,	XKB_KEY_E,		c_set_scale_mode,	{ .i = SCALE_WIDTH } },
	{ 0,		XKB_KEY_f,		c_toggle_fullscreen,	{ .i = 0 } },
	{ 0,		XKB_KEY_b,		c_toggle_bar,		{ .i = 0 } },
	{ 0,		XKB_KEY_g,		c_first_image,		{ .i = 0 } },
	{ ShiftMask,	XKB_KEY_G,		c_n_or_last_image,	{ .i = 0 } },
	{ 0,		XKB_KEY_r,		c_reload_image,		{ .i = 0 } },
	{ ShiftMask,	XKB_KEY_D,		c_remove_image,		{ .i = 0 } },
	{ 0,		XKB_KEY_h,		c_scroll,		{ .i = DIRECTION_LEFT } },
	{ 0,		XKB_KEY_Left,		c_scroll,		{ .i = DIRECTION_LEFT } },
	{ 0,		XKB_KEY_j,		c_scroll,		{ .i = DIRECTION_DOWN } },
	{ 0,		XKB_KEY_Down,		c_scroll,		{ .i = DIRECTION_DOWN } },
	{ 0,		XKB_KEY_k,		c_scroll,		{ .i = DIRECTION_UP } },
	{ 0,		XKB_KEY_Up,		c_scroll,		{ .i = DIRECTION_UP } },
	{ 0,		XKB_KEY_l,		c_scroll,		{ .i = DIRECTION_RIGHT } },
	{ 0,		XKB_KEY_Right,		c_scroll,		{ .i = DIRECTION_RIGHT } },
	{ ControlMask,	XKB_KEY_h,		c_scroll_to_edge,	{ .i = DIRECTION_LEFT } },
	{ ControlMask,	XKB_KEY_Left,		c_scroll_to_edge,	{ .i = DIRECTION_LEFT } },
	{ ControlMask,	XKB_KEY_j,		c_scroll_to_edge,	{ .i = DIRECTION_DOWN } },
	{ ControlMask,	XKB_KEY_Down,		c_scroll_to_edge,	{ .i = DIRECTION_DOWN } },
	{ ControlMask,	XKB_KEY_k,		c_scroll_to_edge,	{ .i = DIRECTION_UP } },
	{ ControlMask,	XKB_KEY_Up,		c_scroll_to_edge,	{ .i = DIRECTION_UP } },
	{ ControlMask,	XKB_KEY_l,		c_scroll_to_edge,	{ .i = DIRECTION_RIGHT } },
	{ ControlMask,	XKB_KEY_Right,		c_scroll_to_edge,	{ .i = DIRECTION_RIGHT } },
	{ 0,		XKB_KEY_plus,		c_zoom,			{ .i = +1 } },
	{ 0,		XKB_KEY_KP_Add,		c_zoom,			{ .i = +1 } },
	{ 0,		XKB_KEY_minus,		c_zoom,			{ .i = -1 } },
	{ 0,		XKB_KEY_KP_Subtract,	c_zoom,			{ .i = -1 } },
	{ 0,		XKB_KEY_equal,		c_set_zoom,		{ .f = 100.0f } },
	{ 0,		XKB_KEY_m,		c_toggle_image_mark,	{ .i = 0 } },
	{ ShiftMask,	XKB_KEY_M,		c_mark_range,		{ .i = 0 } },
	{ ControlMask,	XKB_KEY_m,		c_reverse_marks,	{ .i = 0 } },
	{ ControlMask,	XKB_KEY_u,		c_unmark_all,		{ .i = 0 } },
	{ ShiftMask,	XKB_KEY_N,		c_navigate_marked,	{ .i = +1 } },
	{ ShiftMask,	XKB_KEY_P,		c_navigate_marked,	{ .i = -1 } },
	{ 0,		XKB_KEY_space,		c_navigate,		{ .i = +1 } },
	{ 0,		XKB_KEY_n,		c_navigate,		{ .i = +1 } },
	{ 0,		XKB_KEY_p,		c_navigate,		{ .i = -1 } },
	// gamma
	// rotate
	// flip
	// alpha
	{ 0,		XKB_KEY_a,		c_toggle_antialias,	{ .i = 0 } },
};
