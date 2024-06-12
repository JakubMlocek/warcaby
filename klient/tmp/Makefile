# Compiler to use
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -std=c99
# Target executable names
#SERVER = server
#CLIENT = player
TEST = test_linux
# Source files
#CLIENT_SRC = client_side.c
#SERVER_SRC = server_side.c
TEST_SRC = test.c
# Object files
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
# Default target
# all: $(SERVER) $(CLIENT) 
all: $(TEST)
# Compile server
# $(SERVER): $(SERVER_OBJ)
#        $(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ)
# Compile client
#$(CLIENT): $(CLIENT_OBJ)
#        $(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)
$(TEST): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST) $(TESTOBJ)
# Clean up build files

clean:
	rm -f $(TEST) $(TEST_OBJ)
#        rm -f $(SERVER) $(CLIENT) $(SERVER_OBJ) $(CLIENT_OBJ)
# Compile individual .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: all clean

