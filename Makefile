CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = heavy_hitter
SRC = heavy_hitter.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lpthread

clean:
	rm -f $(TARGET)

.PHONY: all clean
