CC=gcc
CFLAGS=-O3 -std=c99
TARGET=dark64
all: $(TARGET)

$(TARGET): dark64_crypt.c
	$(CC) dark64_crypt.c $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
