CC=gcc
CFLAGS=-Wall -std=gnu11
PROGRAM=flang
PROGRAM_SRC=cli

OBJ_DIR=./obj
BIN_DIR=./bin
SRC_DIR=./src

FILES=data_structures ast tokenizer

DEPS=$(FILES:%=$(SRC_DIR)/%.h)
OBJ=$(FILES:%=$(OBJ_DIR)/%.o)

all: dirs $(OBJ) $(PROGRAM)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJ_DIR)/$(PROGRAM_SRC).o $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(PROGRAM) $^

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)