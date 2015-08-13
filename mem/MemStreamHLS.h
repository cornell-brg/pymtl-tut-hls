//========================================================================
// MemStreamHLS.h
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// Header for testing the accelerator with a pure C++ flow.

#include "MemMsg.h"
#include "XcelMsg.h"

#include <hls_stream.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  extern TestMem MemStreamHLS_mem;
#endif

void MemStreamHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp
);

