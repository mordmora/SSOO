EXEC = forks

#directorios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

CC = gcc
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -std=c99

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(BUILD_DIR) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR): 
	mkdir -p $(BUILD_DIR)

clean: 
	rm -rf $(BUILD_DIR) $(EXEC)
