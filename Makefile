# SPDX-License-Identifier: MIT
# Author:  Giovanni Santini
# Mail:    giovanni.santini@proton.me
# License: MIT

#
# Compiler flags
#
CFLAGS        = -Wall -Werror -Wextra -Wpedantic -std=c99 \
                $(shell llvm-config --cflags)
DEBUG_FLAGS   = -ggdb
RELEASE_FLAGS = -DCOMMIT_HASH=$(shell git rev-parse --verify HEAD)
LDFLAGS       = $(shell llvm-config --ldflags --system-libs --libs core)
CC?           = gcc

#
# Project files
#
OUT_NAME = lang
LIB      = ast.o \
           tokenizer.o \
           token_stream.o \
           parser.o \
           codegen.o
OBJ      = lang.o
TEST_OBJ =

include tests/make.conf

#
# Commands
#
all: $(OUT_NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(OUT_NAME)
release: CFLAGS += $(RELEASE_FLAGS) 
release: $(OUT_NAME)

run: $(OUT_NAME)
	chmod +x $(OUT_NAME)
	./$(OUT_NAME)

clean:
	rm -f $(OBJ) $(LIB) $(TEST_OBJ)

distclean:
	rm -f $(OUT_NAME) $(OUT_NAME)_test

.PHONY: disas
disas: # Disassemble test bitcode
	llvm-dis test.bc

test: CFLAGS += $(DEBUG_FLAGS)
test: $(TEST_OBJ) $(LIB)
	$(CC) $(TEST_OBJ) $(LIB) $(LDFLAGS) $(CFLAGS) -o $(OUT_NAME)_test

check: test
	./$(OUT_NAME)_test

$(OUT_NAME): $(OBJ) $(LIB)
	$(CC) $(OBJ) $(LIB) $(LDFLAGS) $(CFLAGS) -o $(OUT_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
