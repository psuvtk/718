CC = gcc
CFLAGS = --std=gnu99
LDFLAGS = -lm
OBJS = main.o radar_device.o bytearray.o radar_packet.o

srrled: $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) $(LDFLAGS) -o srrled.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS)
