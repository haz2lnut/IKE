CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lconfig
RM = rm

BUILD_DIR = .build
SRC_DIRS = core utils network log sa
INCLUDE_DIRS = $(addprefix -I, $(SRC_DIRS))

SRCS = $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
OBJS = $(SRCS:.c=.o)
OBJS := $(patsubst %.o, $(BUILD_DIR)/%.o, $(OBJS))

USER = $(shell whoami)
ifeq ($(USER), root)
	BEAR =
else
	INCLUDE_DIRS += -I/opt/homebrew/include
	LDFLAGS += -L/opt/homebrew/lib
	BEAR = bear --
endif

TARGET = $(BUILD_DIR)/ike

all: build

build:
	$(BEAR) make $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<

clean:
	$(RM) -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run build
