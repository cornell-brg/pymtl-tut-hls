//========================================================================
// MemProxyHLS.h
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// Header for testing the accelerator with a pure C++ flow.

#ifndef MEM_MEM_PROXY_HLS_H
#define MEM_MEM_PROXY_HLS_H

#include "mem/MemMsg.h"
#include "mem/MemCommon.h"
#include "xcel/XcelMsg.h"

#include <hls_stream.h>

void MemProxyHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp,
  mem::MemReqStream&              memreq,
  mem::MemRespStream&             memresp
);

#endif /* MEM_MEM_PROXY_HLS_H */

