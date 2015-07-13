# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS=-DBIT_ACCURATE -DCPP_COMPILE -I../hls/include

all: transform square

transform: TransformUnitHLS.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

square: Square.cpp
	g++ -I${VHLS_INC} $(CFLAGS) $< -o $@

hls_transform:
	vivado_hls run_hls_transform.tcl

hls_square:
	vivado_hls run_hls_square.tcl

clean:
	rm -f transform square
	rm -rf transform.prj square.prj
