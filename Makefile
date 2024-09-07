CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu89
TARGET = hsh

all: $(TARGET)

$(TARGET): main.o
    $(CC) $(CFLAGS) -o $(TARGET) main.o

clean:
    rm -f *.o $(TARGET)

