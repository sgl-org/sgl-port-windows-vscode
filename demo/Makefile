TARGET    := sgl_simulator
BUILD_DIR := build

# toolchain
CC_PREFIX ?=
CC = $(CC_PREFIX)gcc
AS = $(CC_PREFIX)gcc -x assembler-with-cpp
copy = $(CC_PREFIX)objcopy
SZ = $(CC_PREFIX)size
OD = $(CC_PREFIX)objdump
HEX = $(copy) -O ihex
BIN = $(copy) -O binary -S

CPATH     := -Isdl/include/SDL2      \
			 -I../sgl/source         \
			 -I../sgl/source/include

# NOTE: keep flags in sync with the CMake build (CMakelists.txt at the repo root).
# The old file used -nostdlib -ffreestanding -Werror which breaks the SDL2
# simulator; the working CMake build only uses -g -std=gnu99.
CFLAGS    := $(CPATH) -O0 -g -std=gnu99 -Wall
LDFLAGS   := -lmingw32 -lSDL2main -lSDL2 -Lsdl/lib -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

# Host shell helpers: MinGW make on Windows runs recipes through cmd.exe,
# where Unix tools like cp/rm do not exist.  Detect the OS once and pick the
# portable equivalents; override these on POSIX if needed.
ifeq ($(OS),Windows_NT)
COPY_CMD ?= cmd /c copy /Y
RM_CMD   ?= cmd /c rmdir /s /q
MKDIR_P  ?= cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
# test runner loop: run each test_*.exe and propagate a failure exit code
TEST_RUNNER  = set FAILED=& for %%t in ($(TEST_NAMES)) do @( \
	  echo. & \
	  echo ###################### test: %%t ###################### & \
	  $(BUILD_DIR)\test_%%t.exe || set FAILED=1 & ) & \
	if not defined FAILED (echo ALL TESTS PASSED & exit /b 0) else (echo SOME TESTS FAILED & exit /b 1)
else
COPY_CMD ?= cp
RM_CMD   ?= rm -rf
MKDIR_P  ?= mkdir -p $(BUILD_DIR)
TEST_RUNNER  = rc=0; for t in $(TEST_NAMES); do \
	  echo ""; \
	  echo "###################### test: $$t ######################"; \
	  ./$(BUILD_DIR)/test_$$t.exe || rc=1; \
	done; \
	echo ""; \
	if [ $$rc -eq 0 ]; then echo "ALL TESTS PASSED"; else echo "SOME TESTS FAILED"; fi; \
	exit $$rc
endif

# demo + examples sources
# NOTE: test.c (big gImage_test array demo) and transit_demo.c were removed;
# main.c no longer calls them.
SOURCE    := main.c sgl_port_sdl2.c bg.c                        \
			../sgl/source/components/bench/sgl_bench.c        \
			../sgl/examples/button.c                          \
			../sgl/examples/label.c                           \
			../sgl/examples/label_ext.c                       \
			../sgl/examples/span.c                            \
			../sgl/examples/textedit.c                        \
			../sgl/examples/rect.c                            \
			../sgl/examples/rect_ext.c                        \
			../sgl/examples/img_ext.c                         \
			../sgl/examples/scrollview.c                      \
			../sgl/examples/menu.c                            \
			../sgl/examples/launcher.c                        \
			../sgl/examples/2dball.c                          \
			../sgl/examples/analogclock.c                     \
			../sgl/examples/arc.c                             \
			../sgl/examples/bar.c                             \
			../sgl/examples/battery.c                         \
			../sgl/examples/charge.c                          \
			../sgl/examples/dropdown.c                        \
			../sgl/examples/stepper.c                         \
			../sgl/examples/switch.c                          \
			../sgl/examples/roller.c                          \
			../sgl/examples/slider.c                          \
			../sgl/examples/led.c                             \
			../sgl/examples/scope.c                           \
			../sgl/examples/textlist.c                        \
			../sgl/examples/checkbox.c                        \
			../sgl/examples/msgbox.c                          \
			../sgl/examples/progress.c                        \
			../sgl/examples/physical_key.c                    \
			../sgl/examples/viewlist.c                        \
			../sgl/examples/tabview.c                         \
			../sgl/examples/calendar.c                        \
			../sgl/examples/filedialog.c                      \
			../sgl/examples/filebrowser.c                     \
			../sgl/source/components/games/2048/sgl_2048.c    \
			../images/output/rect_pixmap_imgs.c               \
			../images/output/music_img.c

# sgl core sources
SOURCE    +=                                                   \
			../sgl/source/core/sgl_core.c                     \
			../sgl/source/core/sgl_log.c                      \
			../sgl/source/core/sgl_math.c                     \
			../sgl/source/core/sgl_event.c                    \
			../sgl/source/core/sgl_anim.c                     \
			../sgl/source/core/sgl_fs.c                       \
			../sgl/source/fs/ramfs/sgl_ramfs.c                \
			../sgl/source/fs/posix/sgl_posix.c                \
			../sgl/source/core/sgl_snprintf.c                 \
			../sgl/source/components/timer/sgl_timer.c        \
			../sgl/source/mm/lwmem/lwmem.c                    \
			../sgl/source/mm/lwmem/sgl_mm.c

# sgl draw sources
SOURCE    +=                                                   \
			../sgl/source/draw/sgl_draw.c                     \
			../sgl/source/draw/sgl_draw_arc.c                 \
			../sgl/source/draw/sgl_draw_bezier.c              \
			../sgl/source/draw/sgl_draw_circle.c              \
			../sgl/source/draw/sgl_draw_icon.c                \
			../sgl/source/draw/sgl_draw_line.c                \
			../sgl/source/draw/sgl_draw_ring.c                \
			../sgl/source/draw/sgl_draw_rect.c                \
			../sgl/source/draw/sgl_draw_text.c

# sgl fonts
SOURCE    +=                                                   \
			../sgl/source/fonts/sgl_ascii_consolas14.c        \
			../sgl/source/fonts/sgl_ascii_consolas23.c        \
			../sgl/source/fonts/sgl_ascii_consolas24.c        \
			../sgl/source/fonts/sgl_ascii_consolas32.c        \
			../sgl/source/fonts/sgl_icon_fontawesome14.c      \
			../sgl/source/fonts/sgl_ascii_song23.c

# music demo fonts (generated by sgl_font_conv_cli, kept with the demo)
SOURCE    +=                                                   \
			../sgl/demos/music/sgl_icon_music20.c             \
			../sgl/demos/music/sgl_ascii_consolas17.c          \
			../sgl/demos/music/sgl_ascii_consolas10.c

# sgl widgets
# NOTE: sgl/examples/qrcode.c and sgl/source/widgets/qrcode/qrcode.c would
# collide on the flattened object name qrcode.o, so the example gets an
# explicit object name (ex_qrcode.o) below.
# NOTE: sgl/examples/music_player.c and sgl/demos/music/music_player.c would
# collide on the flattened object name music_player.o, so the demo gets an
# explicit object name (demo_music_player.o) below.
SOURCE    +=                                                   \
			../sgl/source/widgets/2dball/sgl_2dball.c         \
			../sgl/source/widgets/analogclock/sgl_analogclock.c \
			../sgl/source/widgets/arc/sgl_arc.c               \
			../sgl/source/widgets/arc_label/sgl_arc_label.c   \
			../sgl/source/widgets/bar/sgl_bar.c               \
			../sgl/source/widgets/battery/sgl_battery.c       \
			../sgl/source/widgets/box/sgl_box.c               \
			../sgl/source/widgets/button/sgl_button.c         \
			../sgl/source/widgets/calendar/sgl_calendar.c     \
			../sgl/source/widgets/canvas/sgl_canvas.c         \
			../sgl/source/widgets/checkbox/sgl_checkbox.c     \
			../sgl/source/widgets/charge/sgl_charge.c         \
			../sgl/source/widgets/circle/sgl_circle.c         \
			../sgl/source/widgets/coverflow/sgl_coverflow.c   \
			../sgl/source/widgets/dropdown/sgl_dropdown.c     \
			../sgl/source/widgets/filebrowser/sgl_filebrowser.c \
			../sgl/source/widgets/filedialog/sgl_filedialog.c \
			../sgl/source/widgets/gauge/sgl_gauge.c           \
			../sgl/source/widgets/gif/sgl_gif.c               \
			../sgl/source/widgets/icon/sgl_icon.c             \
			../sgl/source/widgets/img/sgl_img.c               \
			../sgl/source/widgets/img_ext/sgl_img_ext.c       \
			../sgl/source/widgets/keyboard/sgl_keyboard.c     \
			../sgl/source/widgets/label/sgl_label.c           \
			../sgl/source/widgets/label_ext/sgl_label_ext.c   \
			../sgl/source/widgets/launcher/sgl_launcher.c     \
			../sgl/source/widgets/led/sgl_led.c               \
			../sgl/source/widgets/line/sgl_line.c             \
			../sgl/source/widgets/menu/sgl_menu.c             \
			../sgl/source/widgets/msgbox/sgl_msgbox.c         \
			../sgl/source/widgets/numberkbd/sgl_numberkbd.c   \
			../sgl/source/widgets/polygon/sgl_polygon.c       \
			../sgl/source/widgets/progress/sgl_progress.c     \
			../sgl/source/widgets/qrcode/sgl_qrcode.c         \
			../sgl/source/widgets/rect/sgl_rect.c             \
			../sgl/source/widgets/rect_ext/sgl_rect_ext.c     \
			../sgl/source/widgets/ring/sgl_ring.c             \
			../sgl/source/widgets/roller/sgl_roller.c         \
			../sgl/source/widgets/scope/sgl_scope.c           \
			../sgl/source/widgets/scrollview/sgl_scrollview.c \
			../sgl/source/widgets/slider/sgl_slider.c         \
			../sgl/source/widgets/span/sgl_span.c             \
			../sgl/source/widgets/sprite/sgl_sprite.c         \
			../sgl/source/widgets/statusbar/sgl_statusbar.c   \
			../sgl/source/widgets/stepper/sgl_stepper.c       \
			../sgl/source/widgets/switch/sgl_switch.c         \
			../sgl/source/widgets/tabview/sgl_tabview.c       \
			../sgl/source/widgets/textbox/sgl_textbox.c       \
			../sgl/source/widgets/textedit/sgl_textedit.c     \
			../sgl/source/widgets/textline/sgl_textline.c     \
			../sgl/source/widgets/textlist/sgl_textlist.c     \
			../sgl/source/widgets/unzip_image/sgl_unzip_image.c \
			../sgl/source/widgets/viewlist/sgl_viewlist.c     \
			../sgl/source/widgets/spectrum/sgl_spectrum.c     \
			../sgl/source/widgets/win/sgl_win.c

.PHONY: config all
all: config $(BUILD_DIR)/$(TARGET).exe elf_info

config:
	@echo "copy sgl_config.h..."
	@$(COPY_CMD) sgl_config.h ..\sgl\source\sgl_config.h


# object list (flattened into BUILD_DIR); two files need explicit object
# names because their basenames would collide on the flattened list:
#   sgl/examples/qrcode.c          -> ex_qrcode.o (qrcode example UI)
#   widgets/qrcode/qrcode.c        -> qr_codec.o  (QR codec library)
# (vpath would resolve the ambiguous qrcode.o to the examples dir first)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.c, %.o, $(SOURCE)))) \
          $(BUILD_DIR)/ex_qrcode.o \
          $(BUILD_DIR)/qr_codec.o \
          $(BUILD_DIR)/demo_music_player.o
vpath %.c $(sort $(dir $(SOURCE)))

QRCODE_EXAMPLE_SRC := ../sgl/examples/qrcode.c
QRCODE_CODEC_SRC   := ../sgl/source/widgets/qrcode/qrcode.c
DEMO_MUSIC_SRC     := ../sgl/demos/music/music_player.c

# sgl/examples/qrcode.c is the qrcode example UI (built under a distinct
# object name); widgets/qrcode/qrcode.c is the QR codec library
$(BUILD_DIR)/ex_qrcode.o: $(QRCODE_EXAMPLE_SRC) Makefile | $(BUILD_DIR)
	@echo "CC   $<"
	@$(CC) -c $(CFLAGS) -MMD -MP \
		-MF  $(BUILD_DIR)/ex_qrcode.d \
		-Wa,-a,-ad,-alms=$(BUILD_DIR)/ex_qrcode.lst $< -o $@

$(BUILD_DIR)/qr_codec.o: $(QRCODE_CODEC_SRC) Makefile | $(BUILD_DIR)
	@echo "CC   $<"
	@$(CC) -c $(CFLAGS) -MMD -MP \
		-MF  $(BUILD_DIR)/qr_codec.d \
		-Wa,-a,-ad,-alms=$(BUILD_DIR)/qr_codec.lst $< -o $@

# sgl/demos/music/music_player.c is the light-theme music player demo
# (built under a distinct object name to avoid collision with examples)
$(BUILD_DIR)/demo_music_player.o: $(DEMO_MUSIC_SRC) Makefile | $(BUILD_DIR)
	@echo "CC   $<"
	@$(CC) -c $(CFLAGS) -MMD -MP \
		-MF  $(BUILD_DIR)/demo_music_player.d \
		-Wa,-a,-ad,-alms=$(BUILD_DIR)/demo_music_player.lst $< -o $@


$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "CC   $<"
	@$(CC) -c $(CFLAGS) -MMD -MP \
		-MF  $(BUILD_DIR)/$(notdir $(<:.c=.d)) \
		-Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@


$(BUILD_DIR)/$(TARGET).exe: $(OBJECTS) Makefile
	@echo "LD   $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ''
	@echo "Build Successful!"
	@echo "ELF   $@"


elf_info: $(BUILD_DIR)/$(TARGET).exe
	@echo "=================================================================="
	@$(SZ) $<
	@echo "=================================================================="


$(BUILD_DIR):
	@$(MKDIR_P)


# Pseudo command
.PHONY: clean run
clean:
	-@$(RM_CMD) $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET).exe
	-@$(COPY_CMD) sdl\bin\SDL2.dll $(BUILD_DIR)\
	@$(BUILD_DIR)\$(TARGET).exe


# ---- headless widget unit tests --------------------------------------------
# Each test reuses the simulator object list minus main.o / sgl_port_sdl2.o
# (both pull in SDL and open a GUI window), plus its own test_<name>.o.
# The tests install their own in-memory framebuffer port, so they need no
# display and no SDL runtime.
#
#   make test              -> run every test_*.c found in this directory
#   make test-span         -> run one specific test
TEST_SRCS    := $(wildcard test_*.c)
TEST_NAMES   := $(patsubst test_%.c,%,$(TEST_SRCS))
TEST_BASE    := $(filter-out $(BUILD_DIR)/main.o $(BUILD_DIR)/sgl_port_sdl2.o,$(OBJECTS))

$(BUILD_DIR)/test_%.o: test_%.c Makefile | $(BUILD_DIR)
	@echo "CC   $<"
	@$(CC) -c $(CFLAGS) -MMD -MP \
		-MF  $(BUILD_DIR)/test_$*.d \
		-Wa,-a,-ad,-alms=$(BUILD_DIR)/test_$*.lst $< -o $@

$(BUILD_DIR)/test_%.exe: $(BUILD_DIR)/test_%.o $(TEST_BASE) Makefile
	@echo "LD   $@"
	@$(CC) $(filter %.o,$^) -o $@

# A static pattern rule is required here: a target declared .PHONY never
# matches an implicit (pattern) rule, so "make test-keys" would otherwise
# report "Nothing to be done".  The static form binds the recipe to each
# concrete name instead.
.PHONY: test $(addprefix test-,$(TEST_NAMES))
$(addprefix test-,$(TEST_NAMES)): test-%: $(BUILD_DIR)/test_%.exe
	@echo "=================================================================="
	@./$(BUILD_DIR)/test_$*.exe

test: $(addprefix $(BUILD_DIR)/test_,$(addsuffix .exe,$(TEST_NAMES)))
	@$(TEST_RUNNER)


-include $(wildcard $(BUILD_DIR)/*.d)
