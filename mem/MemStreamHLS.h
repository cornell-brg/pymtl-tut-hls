//========================================================================
// MemStreamHLS.h
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// Header for testing the accelerator with a pure C++ flow.

#include "MemMsg.h"
#include "MemCommon.h"
#include "XcelMsg.h"

#include <hls_stream.h>

void MemStreamHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp,
  mem::MemReqStream&               memreq,
  mem::MemRespStream&              memresp
);

