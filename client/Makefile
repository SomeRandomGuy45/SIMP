# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -fPIC # -fPIC for shared libraries

# Source files for the library and main executable
LIB_SRCS = $(wildcard simp-helper/*.c)
MAIN_SRC = main.c

# Object files
LIB_OBJS = $(LIB_SRCS:.c=.o)
MAIN_OBJ = $(MAIN_SRC:.c=.o)

# Shared library output name
ifeq ($(OS),Windows_NT)
    LIB_TARGET = libsimp.dll
    LDFLAGS = -shared
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIB_TARGET = libsimp.dylib
        LDFLAGS = -shared
    else
        LIB_TARGET = libsimp.so
        LDFLAGS = -shared
        LINUX_FLAGS = -Wl,-rpath='$$ORIGIN'
    endif
endif

# Final executable
EXECUTABLE = simp

# Default rule to build everything
all: $(EXECUTABLE)

build_thing:
	clang -c simp-helper/cJSON.c -o simp-helper/cJSON.o

# Rule to build the shared library
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(LIB_OBJS) $(LDFLAGS) -lcurl -o $(LIB_TARGET)

# Rule to compile the main program and link with the library
$(EXECUTABLE): $(MAIN_OBJ) $(LIB_TARGET)
	$(CC) $(MAIN_OBJ) -L. -lsimp $(LINUX_FLAGS) -o $(EXECUTABLE)

# Rule to compile .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) simp-helper/cJSON.o -c $< -o $@

# Clean up build files
clean:
	rm -rf $(LIB_OBJS) $(MAIN_OBJ) $(LIB_TARGET) $(EXECUTABLE) *.o src/*.o simp-helper/cJSON.o