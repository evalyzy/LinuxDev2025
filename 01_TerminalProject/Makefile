CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lncursesw
TARGET = show
SRC = show.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

