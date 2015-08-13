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
  extern TestMem SortXcelHLS_mem;
#endif

using namespace xcel;
using namespace mem;

void SortXcelHLS(
  hls::stream<XcelReqMsg>&  xcelreq,
  hls::stream<XcelRespMsg>& xcelresp
);

