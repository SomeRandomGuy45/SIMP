# Compiler and flags
CC = clang
CFLAGS = -Wall -g -fPIC # -fPIC for shared libraries

# Source files for the library and main executable
LIB_SRCS = $(wildcard simp-helper/*.c)
MAIN_SRC = main.c

# Object files
LIB_OBJS = $(LIB_SRCS:.c=.o)
MAIN_OBJ = $(MAIN_SRC:.c=.o)

# Shared library output name
ifeq ($(OS),Windows_NT)
    LIB_TARGET = libsimplib.dll
    LDFLAGS = -shared
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIB_TARGET = libsimplib.dylib
        LDFLAGS = -shared
    else
        LIB_TARGET = libsimplib.so
        LDFLAGS = -shared
    endif
endif

# Final executable
EXECUTABLE = simp

# Default rule to build everything
all: $(EXECUTABLE)

# Rule to build the shared library
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(LIB_OBJS) $(LDFLAGS) -lcurl -o $(LIB_TARGET)

# Rule to compile the main program and link with the library
$(EXECUTABLE): $(MAIN_OBJ) $(LIB_TARGET)
	$(CC) $(MAIN_OBJ) -L. -lsimplib -o $(EXECUTABLE)

# Rule to compile .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(LIB_OBJS) $(MAIN_OBJ) $(LIB_TARGET) $(EXECUTABLE) *.o src/*.o