CC=g++

VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include
HFLAGS=-DBIT_ACCURATE -I${VHLS_INC}
CFLAGS=-DCPP_COMPILE

HEADERS=RbTree.h
LIST=RbTree
#INSERT=RbTreeInsertHLS
PROGS=$(LIST) $(INSERT)

all: $(PROGS)

$(LIST): $(LIST).t.cc $(HEADERS)
	$(CC) $(CFLAGS) $(HFLAGS) $< -o $@

#$(INSERT): $(INSERT).t.cc $(HEADERS)
#	$(CC) $(CFLAGS) $(HFLAGS) $< -o $@

#hls: $(INSERT).t.cc run_hls.tcl
#	vivado_hls run_hls.tcl

clean:
	rm -f *.o
	rm -f $(PROGS)
	rm -rf *.prj

