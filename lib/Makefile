CC65_ROOT = /Users/slembcke/Development/NES/cc65

CC = $(CC65_ROOT)/bin/cc65
AS = $(CC65_ROOT)/bin/ca65
LD = $(CC65_ROOT)/bin/ld65
AR = $(CC65_ROOT)/bin/ar65

SRC = \
	coroutine.s \
	rand8.s \
	boot.s \
	zeropage.s \
	nmi.s \
	wait_frames.s \
	uxrom.s \
	buffer.s \
	buffer_inc.s \
	buffer_color.s \
	buffer_data.s \
	inc.s \
	fill.s \
	blit.s \
	sprite.s \
	lz4.s \
	lz4_to_vram.s \
	lz4_to_ram.s \

OBJS = $(SRC:.s=.o)

default: px.lib

px.lib: $(OBJS)
	$(AR) r $@ $^

%.o: %.s
	$(AS) $< -o $@

clean:
	-rm $(OBJS) px.lib

.phony: clean
