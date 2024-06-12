# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Executable name
TARGET = server

# Source files
SRCS = server.c client.c warcaby.c

# Header files
HDRS = warcaby.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean