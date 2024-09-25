BIN_NAME    = task1_6
SRC_DIR     = src
BIN_DIR     = bin
LIB_DIR     = $(SRC_DIR)/lib
BIN_LIB_DIR = $(BIN_DIR)/lib
LIB_NAME    = libmythread.a

INCLUDE_DIR = $(LIB_DIR)
CC = clang
CFLAGS = -Wall -Wextra -Werror=implicit-function-declaration -Wpedantic -O2 -g

LIB_SRC_FILES = $(LIB_DIR)/mythread.c
LIB_OBJ_FILES = $(LIB_SRC_FILES:.c=.o)

SRC_FILES = $(shell find $(SRC_DIR) -type f -name "*.c" ! -name "mythread.c")
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))

INCLUDE_PATHS = -I$(INCLUDE_DIR)

all: $(BIN_DIR)/$(BIN_NAME)

build: $(BIN_DIR)/$(BIN_NAME)

$(BIN_DIR)/$(BIN_NAME): $(OBJ_FILES) $(BIN_LIB_DIR)/$(LIB_NAME)
	@echo "CC 		| $@"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $(OBJ_FILES) -L$(BIN_LIB_DIR) -lmythread
	@rm -f $(OBJ_FILES)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "CC 		| $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(BIN_LIB_DIR)/$(LIB_NAME): $(LIB_OBJ_FILES)
	@echo "AR 		| $@"
	@mkdir -p $(BIN_LIB_DIR)
	@ar rcs $@ $^
	@rm -f $(LIB_OBJ_FILES) 

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	@echo "CC 		| $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Clean..."
	@rm -rf $(BIN_DIR)/*.o $(LIB_DIR)/*.o $(BIN_LIB_DIR)/$(LIB_NAME)

run: all
	@echo "Running $(BIN_NAME)" ; echo
	@./$(BIN_DIR)/$(BIN_NAME)

