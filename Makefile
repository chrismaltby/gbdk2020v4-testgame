CC = lcc
ROM_BUILD_DIR = build
CART_SIZE = 16
COLOR =
DEBUG = 1
OBJDIR = obj
CFLAGS = -Iinclude
LFLAGS_NBANKS += -Wl-yo$(CART_SIZE) -Wl-ya4
LFLAGS = -Wl-yt27 $(LFLAGS_NBANKS)

ifdef RELEASE
CFLAGS += -Wf'--max-allocs-per-node 50000'
LFLAGS += -Wf--debug -Wl-m -Wl-w -Wl-y -Wl-j
endif

ifdef DEBUG
# CFLAGS += -Wf--debug -Wf--nolospre -Wf--nogcse -Wl-m -Wl-w -Wl-y
CFLAGS += -Wf--debug -Wf--nogcse -Wl-m -Wl-w -Wl-y
LFLAGS += -Wf--debug -Wl-m -Wl-w -Wl-y -Wl-j
endif

ifdef COLOR
CFLAGS += -DCGB
LFLAGS += -Wm-yC
endif

ifdef PROFILE
CFLAGS += -Wf--profile
endif

all: $(ROM_BUILD_DIR)/working.gb $(ROM_BUILD_DIR)/failing.gb

$(OBJDIR)/%.o:	src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ROM_BUILD_DIR)/%.gb:	obj/%.o
	mkdir -p $(ROM_BUILD_DIR)
	$(CC) $(LFLAGS) -o $@ $^

clean:
	rm -rf obj/*
	rm -rf $(ROM_BUILD_DIR)

rom: $(ROM_BUILD_DIR)/game.gb
