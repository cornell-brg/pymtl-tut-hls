//========================================================================
// MemStreamHLS.h
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// Header for testing the accelerator with a pure C++ flow.

#ifndef XMEM_MEM_STREAM_HLS_H
#define XMEM_MEM_STREAM_HLS_H

#include "xmem/MemMsg.h"
#include "xmem/MemCommon.h"
#include "xcel/XcelMsg.h"

#include <hls_stream.h>

void MemStreamHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp,
  xmem::MemReqStream&             memreq,
  xmem::MemRespStream&            memresp
);

#endif /* XMEM_MEM_STREAM_HLS_H */

