# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS=-DBIT_ACCURATE

main: main.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

hls:
	vivado_hls run_hls.tcl

clean:
	rm -f main
	rm -rf hls.prj
