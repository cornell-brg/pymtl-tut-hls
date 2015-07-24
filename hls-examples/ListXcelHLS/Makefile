CC=g++
CFLAGS=

PROG=ListXcelHLS

all: $(PROG)

$(PROG).t.o: $(PROG).t.cc
	$(CC) $(CFLAGS) $? -c -o $@

$(PROG): $(PROG).t.o
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -f *.o 
	rm -f $(PROG)

