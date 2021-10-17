#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

#include <xkbcommon/xkbcommon.h>
#ifdef __linux__
#include <linux/input-event-codes.h>
#elif __FreeBSD__
#include <dev/evdev/input-event-codes.h>
#endif
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

#include "client_state.h"

#define NoMask (0)
#define ShiftMask (1<<0)
#define LockMask (1<<1)
#define ControlMask (1<<2)
#define Mod1Mask (1<<3)
#define Mod2Mask (1<<4)
#define Mod3Mask (1<<5)
#define Mod4Mask (1<<6)

void connect_display(struct client_state *state);
void disconnect_display(struct client_state *state);
