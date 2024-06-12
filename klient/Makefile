# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Executable name
TARGET = compiled/client

# Source and header files
SRCDIR = src
BUILDDIR = compiled
SRCS = $(wildcard $(SRCDIR)/*.c)
HDRS = $(wildcard $(SRCDIR)/*.h)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HDRS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up object files and the executable
clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

# Phony targets
.PHONY: all clean