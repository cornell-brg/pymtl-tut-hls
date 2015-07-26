CC=g++

VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include
HFLAGS=-DBIT_ACCURATE -I${VHLS_INC}
CFLAGS=-DCPP_COMPILE

LIST=ListHLS
INSERT=ListInsertHLS
PROGS=$(LIST) $(INSERT)

all: $(PROGS)

$(LIST): $(LIST).t.cc $(LIST).h
	$(CC) $(CFLAGS) $< -o $@

$(INSERT): $(INSERT).t.cc $(LIST).h
	$(CC) $(CFLAGS) $(HFLAGS) $< -o $@

hls: $(INSERT).t.cc run_hls.tcl
	vivado_hls run_hls.tcl

clean:
	rm -f *.o
	rm -f $(PROGS)
	rm -rf *.prj

