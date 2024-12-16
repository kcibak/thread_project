CC = gcc
CFLAGS = -Wall -pthread
TARGET = cv

$(TARGET): cv.o
	$(CC) $(CFLAGS) -o $(TARGET) cv.o

cv.o: cv.c cv.h
	$(CC) $(CFLAGS) -c cv.c

clean:
	rm -f *.o $(TARGET)