
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99 -fPIC
LDFLAGS = -lrt

# Source files
SRCS = src/store.c src/types.c src/utils.c
OBJS = $(SRCS:.c=.o)

# Library names
STATIC_LIB = libdatastore.a
SHARED_LIB = libdatastore.so

# Demo program
DEMO = examples/demo
DEMO_SRC = examples/demo.c

.PHONY: all clean

all: $(STATIC_LIB) $(SHARED_LIB) $(DEMO)

# Create the static library
$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

# Create the shared library
$(SHARED_LIB): $(OBJS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the demo program
$(DEMO): $(DEMO_SRC) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(DEMO_SRC) -L. -ldatastore $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f src/*.o $(STATIC_LIB) $(SHARED_LIB) $(DEMO)
