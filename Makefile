# iv - image viewer
# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
BIN = $(BIN_DIR)/image-viewer

all: options $(BIN)

options:
	@echo iv build options:
	@echo "CFLAGS  = $(IVCFLAGS)"
	@echo "LDFLAGS = $(IVLDFLAGS)"
	@echo "CC      = $(CC)"

config.h:
	cp config.def.h config.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo test
	$(CC) $(IVCFLAGS) -c $< -o $@

$(OBJDIR)/main.o: include/options.h include/client_state.h include/fileinfo.h include/display.h include/window.h include/config.h
$(OBJDIR)/options.o: include/options.h include/util.h
$(OBJDIR)/fileinfo.o: include/client_state.h include/fileinfo.h
$(OBJDIR)/display.o: include/client_state.h include/display.h include/config.h
$(OBJDIR)/window.o: include/client_state.h include/window.h
$(OBJDIR)/texture.o: include/client_state.h include/texture.h include/shader.h
$(OBJDIR)/image.o: include/image.h
$(OBJDIR)/bar.o: include/client_state.h include/bar.h include/shader.h include/font.h
$(OBJDIR)/shader.o: include/shader.h
$(OBJDIR)/font.o: include/font.h
$(OBJDIR)/camera.o: include/camera.h
$(OBJDIR)/commands.o: include/commands.h

xdg-shell-client-protocol.o: include/xdg-shell-client-protocol.h
xdg-decoration-client-protocol.o: include/xdg-decoration-protocol.h

$(OBJ): config.mk

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(IVLDFLAGS)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all options clean
