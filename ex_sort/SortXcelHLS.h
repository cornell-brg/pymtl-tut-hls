//========================================================================
// SortXcelHLS.h
//========================================================================
// Author  : Christopher Batten
// Date    : July 2, 2015
//
// Header for testing the accelerator with a pure C++ flow.

#include "../mem/MemMsg.h"
#include "../xcel/XcelMsg.h"
#include <hls_stream.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  extern mem::TestMem SortXcelHLS_mem;
#endif

void SortXcelHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp
);

