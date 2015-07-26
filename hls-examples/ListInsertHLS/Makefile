CC=g++

VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include
HFLAGS=-DBIT_ACCURATE -I${VHLS_INC}
CFLAGS=-DCPP_COMPILE

PROG=ListProxy
INSERT=ListInsertHLS

all: $(PROG) ListInsertHLS

$(PROG).t.o: $(PROG).t.cc $(PROG).h
	$(CC) $(CFLAGS) $< -c -o $@

$(PROG): $(PROG).t.o
	$(CC) $(CFLAGS) $? -o $@

$(INSERT): $(INSERT).cpp $(PROG).h
	$(CC) $(CFLAGS) $(HFLAGS) $< -o $@

hls: ListInsertHLS.cpp run_hls.tcl
	vivado_hls run_hls.tcl

clean:
	rm -f *.o 
	rm -f $(PROG) $(INSERT)

