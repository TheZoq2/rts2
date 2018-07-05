SOC_HOME=/local/rts2/starburst_32cores
include $(SOC_HOME)/src/os/Makefile_app.inc
PROJECT=main
SRC=$(addsuffix .cc,$(PROJECT)) graphics_thread.cc read_thread.cc fourier_thread.cc trigger_thread.cc graphics_command.cc fourier_draw_thread.cc kissfft/kiss_fft.c kissfft/kiss_fftr.c graphics_util.cc util.cc
# SRC=main.cc graphics_thread.cc

CFLAGS=-Wno-error=unused-variable

.PHONY: all
.DEFAULT_GOAL: all

all: $(PROJECT).nc $(PROJECT).xmd
	./$(PROJECT).nc
#	./$(PROJECT).xmd

$(PROJECT).elf: $(SRC)

