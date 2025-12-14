# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Source files
SRC_BASE = maze.c
SRC_V1 = maze_v1.c
SRC_V2 = maze_v2.c
SRC_V3 = maze_v3.c

# Binary files
BIN_BASE = maze
BIN_V1 = maze_v1
BIN_V2 = maze_v2
BIN_V3 = maze_v3

# Patches
PATCH1 = 0001-size-from-cli.patch
PATCH2 = 0002-symbols-from-cli.patch
PATCH3 = 0003-seed-from-cli.patch

all: $(BIN_BASE) $(BIN_V1) $(BIN_V2) $(BIN_V3)

# 编译 Base
$(BIN_BASE): $(SRC_BASE)
	$(CC) $(CFLAGS) -o $@ $<

# 生成并编译 V1
$(SRC_V1): $(SRC_BASE) $(PATCH1)
	cp $(SRC_BASE) $(SRC_V1)
	patch $(SRC_V1) < $(PATCH1)

$(BIN_V1): $(SRC_V1)
	$(CC) $(CFLAGS) -o $@ $<

# 生成并编译 V2
$(SRC_V2): $(SRC_V1) $(PATCH2)
	cp $(SRC_V1) $(SRC_V2)
	patch $(SRC_V2) < $(PATCH2)

$(BIN_V2): $(SRC_V2)
	$(CC) $(CFLAGS) -o $@ $<

# 生成并编译 V3
$(SRC_V3): $(SRC_V2) $(PATCH3)
	cp $(SRC_V2) $(SRC_V3)
	patch $(SRC_V3) < $(PATCH3)

$(BIN_V3): $(SRC_V3)
	$(CC) $(CFLAGS) -o $@ $<

run: all
	@echo "\n--- 1. Base (Default 6x6) ---"
	./$(BIN_BASE)
	@echo "\n--- 2. V1 (Size 8x8) ---"
	./$(BIN_V1) 8
	@echo "\n--- 3. V2 (Symbols '@ ', Size 10) ---"
	./$(BIN_V2) "@ " 10
	@echo "\n--- 4. V3 (Seed 12345, Symbols '.#', Size 7) ---"
	./$(BIN_V3) 12345 ".#" 7

clean:
	rm -f maze_v1.c maze_v2.c maze_v3.c
	rm -f maze maze_v1 maze_v2 maze_v3
	rm -f *.rej *.orig

.PHONY: all run clean
