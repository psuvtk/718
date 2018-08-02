SRC=RadarPlusAIS.c

CC=g++
LDFLAG=-lpthread

RadarPlusAis.run: $(SRC)
	$(CC) $(SRC) $(LDFLAG) -o $@

.PHONY: clean
clean:
	rm *.o
