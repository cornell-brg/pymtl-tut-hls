//========================================================================
// SortXcelHLS.h
//========================================================================
// Header for testing the accelerator with a pure C++ flow.

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

#include "../mem/MemMsg.h"
#include "../mem/ArrayMemPortAdapter.h"

#include "../xcel/XcelMsg.h"
#include "../xcel/XcelWrapper.h"

#ifdef XILINX_VIVADO_HLS_TESTING
#include "../mem/TestMem.h"
#endif

#ifdef XILINX_VIVADO_HLS_TESTING
void SortXcelHLS
(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp,
  mem::TestMem&                   memreq,
  mem::TestMem&                   memresp
);
#else
void SortXcelHLS
(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp,
  hls::stream<mem::MemReqMsg>&    memreq,
  hls::stream<mem::MemRespMsg>&   memresp
);
#endif
