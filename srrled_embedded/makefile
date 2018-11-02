CC = gcc
CFLAGS = --std=gnu99
OBJS = main.o radar_device.o

srrled: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o srrled.out

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

radar_device.o: radar_device.c
	$(CC) $(CFLAGS) -c radar_device.c -o radar_device.o

.PHONY: clean
clean:
	rm -f $(OBJS)
