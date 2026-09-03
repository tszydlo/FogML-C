CC = gcc

FOGML_SDK_DIR = lib/fogml_sdk

# fogml_sdk/ports/fogml_ports.h only pulls in <math.h>/<stdbool.h>/<stddef.h>/<string.h>
# under __ZEPHYR__ (Arduino.h supplies the Arduino equivalents); force-include them here
# for this plain host build instead of patching the vendored submodule.
CFLAGS_COMMON = -Wall -Wextra -O2 -std=c11 \
                -include math.h -include stdbool.h -include stddef.h -include string.h
LDLIBS = -lm

INCS = -Isrc \
       -Ilib \
       -I$(FOGML_SDK_DIR) \
       -I$(FOGML_SDK_DIR)/anomaly \
       -I$(FOGML_SDK_DIR)/anomaly_rt \
       -I$(FOGML_SDK_DIR)/classifier \
       -I$(FOGML_SDK_DIR)/dsp \
       -I$(FOGML_SDK_DIR)/dsp/kissfft \
       -I$(FOGML_SDK_DIR)/ports \
       -I$(FOGML_SDK_DIR)/rl \
       -I$(FOGML_SDK_DIR)/scaler

SRC = src/main.c \
      $(FOGML_SDK_DIR)/ports/fogml_ports_posix.c \
      $(FOGML_SDK_DIR)/anomaly/fogml_anomaly.c \
      $(FOGML_SDK_DIR)/anomaly_rt/fogml_lof.c \
      $(FOGML_SDK_DIR)/anomaly_rt/fogml_reservoir.c \
      $(FOGML_SDK_DIR)/dsp/fogml_dsp.c \
      $(FOGML_SDK_DIR)/dsp/kissfft/kiss_fft.c \
      $(FOGML_SDK_DIR)/dsp/kissfft/kiss_fftr.c \
      $(FOGML_SDK_DIR)/scaler/fogml_scaler.c

BUILDDIR = build
OUTDIR = bin
NAME = fogml_c

RELEASE_DIR = $(BUILDDIR)/release
DEBUG_DIR = $(BUILDDIR)/debug

OBJ_RELEASE = $(patsubst %.c,$(RELEASE_DIR)/%.o,$(SRC))
OBJ_DEBUG = $(patsubst %.c,$(DEBUG_DIR)/%.o,$(SRC))

# `make`       -> release build, no debug info, objects under build/release
build: $(OBJ_RELEASE) | $(OUTDIR)
	$(CC) $(CFLAGS_COMMON) -o $(OUTDIR)/$(NAME) $(OBJ_RELEASE) $(LDLIBS)

# `make debug` -> debug build (-g), objects under build/debug
debug: $(OBJ_DEBUG) | $(OUTDIR)
	$(CC) $(CFLAGS_COMMON) -g -o $(OUTDIR)/$(NAME) $(OBJ_DEBUG) $(LDLIBS)

$(RELEASE_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_COMMON) $(INCS) -c -o $@ $<

$(DEBUG_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_COMMON) -g $(INCS) -c -o $@ $<

$(OUTDIR):
	mkdir -p $(OUTDIR)

run: build
	./$(OUTDIR)/$(NAME)

clean:
	rm -rf $(BUILDDIR) $(OUTDIR)

.PHONY: build debug run clean
