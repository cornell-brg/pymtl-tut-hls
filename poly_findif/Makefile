# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS=-DBIT_ACCURATE -I../hls/include

all: findif

findif: FindIfUnitHLS.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

hls_findif:
	vivado_hls run_hls_findif.tcl

clean:
	rm -f findif eqzero
	rm -rf findif.prj eqzero.prj
