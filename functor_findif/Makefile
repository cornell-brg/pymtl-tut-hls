# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS=-DBIT_ACCURATE -I../hls/include

FindIfUnitHLS: FindIfUnitHLS.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

findif:
	vivado_hls run_hls_findif.tcl

eqzero:
	vivado_hls run_hls_eqzero.tcl

clean:
	rm -f main
	rm -rf hls.prj
