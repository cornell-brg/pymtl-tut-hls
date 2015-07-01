# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS=-DBIT_ACCURATE -DCPP_COMPILE -I../hls/include

all: findif eqzero

findif: FindIfUnitHLS.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

eqzero: EqZero.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

hls_findif:
	vivado_hls run_hls_findif.tcl

hls_eqzero:
	vivado_hls run_hls_eqzero.tcl

clean:
	rm -f findif eqzero
	rm -rf findif.prj eqzero.prj
