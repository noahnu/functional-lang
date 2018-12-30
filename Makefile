CC=gcc
CFLAGS=-Wall -std=gnu11 -g
PROGRAM=flang
PROGRAM_SRC=cli

OBJ_DIR=./obj
BIN_DIR=./bin
SRC_DIR=./src

FILES=ast ast_debug lexer/charsets lexer/debug lexer/lexer lexer/rules
HEADER_ONLY=lexer/token_types
OBJECT_ONLY=cli

DEPS=$(FILES:%=$(SRC_DIR)/%.h) $(HEADER_ONLY:%=$(SRC_DIR)/%.h)
OBJ=$(FILES:%=$(OBJ_DIR)/%.o) $(OBJECT_ONLY:%=$(OBJ_DIR)/%.o)

all: dirs $(OBJ) $(PROGRAM)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJ_DIR)/$(PROGRAM_SRC).o $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(PROGRAM) $^

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
	rm -f ./*.stackdump

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/lexer