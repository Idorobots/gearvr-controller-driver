CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -O2 -I./include `PKG_CONFIG_PATH=./lib/pkgconfig/ pkg-config --cflags gattlib`
LDLIBS = -L./lib `PKG_CONFIG_PATH=./lib/pkgconfig/ pkg-config --libs gattlib` -pthread
VPATH = ./src

TARGET = gearvr-test
OBJS = gearvr.o

all: $(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) $^ -c -o $@

$(TARGET): $(OBJS)
	$(CC) $^ $(LDLIBS) -o $@

.PHONY: clean

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)
