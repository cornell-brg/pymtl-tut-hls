//========================================================================
// SortXcelHLS.h
//========================================================================
// Header for testing the accelerator with a pure C++ flow.

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

#include "../mem/MemCommon.h"
#include "../mem/MemMsg.h"
#include "../mem/ArrayMemPortAdapter.h"

#include "../xcel/XcelMsg.h"
#include "../xcel/XcelWrapper.h"

#ifdef XILINX_VIVADO_HLS_TESTING
#include "../mem/TestMem.h"
#endif

void SortXcelHLS
(
  hls::stream<xcel::XcelReqMsg>&   xcelreq,
  hls::stream<xcel::XcelRespMsg>&  xcelresp,
  mem::MemReqStream&               memreq,
  mem::MemRespStream&              memresp
);
