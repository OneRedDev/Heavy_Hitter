CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = udp_flood
SRC = heavy_hitter.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lpthread

clean:
	rm -f $(TARGET)

.PHONY: all clean
