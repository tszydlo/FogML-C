CC = gcc

FOGML_SDK_DIR = lib/fogml_sdk

# fogml_sdk/ports/fogml_ports.h only pulls in <math.h>/<stdbool.h>/<stddef.h>/<string.h>
# under __ZEPHYR__ (Arduino.h supplies the Arduino equivalents); force-include them here
# for this plain host build instead of patching the vendored submodule.
CFLAGS = -Wall -Wextra -O2 -std=c11 \
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

OBJDIR = build
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))

OUTDIR = bin
NAME = fogml_c

build: $(OUTDIR)/$(NAME)

$(OUTDIR)/$(NAME): $(OBJ) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

$(OUTDIR):
	mkdir -p $(OUTDIR)

run: build
	./$(OUTDIR)/$(NAME)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)

.PHONY: build run clean
