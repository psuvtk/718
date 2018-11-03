CC = gcc
CFLAGS = --std=gnu99
OBJS = main.o radar_device.o bytearray.o

srrled: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o srrled.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS)
