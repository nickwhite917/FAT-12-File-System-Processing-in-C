CC = gcc
CFLAGS = -std=gnu99 -Wall -g -I.
DEPS = processhandling.h commands.h Fat12BootEntry.h
OBJ = main.c processhandling.c commands.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

proj3: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

run:
	./proj3

.PHONY: clean
clean:
	rm -f *.o *~ proj3
