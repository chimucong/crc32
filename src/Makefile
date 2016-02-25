CC = gcc

CFLAGS =  -g   -Wall -m64 -mpclmul -msse4.2
# -Ofast

TARGET = test

all: $(TARGET)

crc_s1024.o: crc_s1024.c
	$(CC) $(CFLAGS) -c crc_s1024.c
crc_parallel.o: crc_parallel.c
	$(CC) $(CFLAGS) -c crc_parallel.c
$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c
$(TARGET): $(TARGET).o crc_s1024.o crc_parallel.o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o crc_s1024.o crc_parallel.o

clean:
	rm $(TARGET)  *.o
