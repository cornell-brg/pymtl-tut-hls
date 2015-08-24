//========================================================================
// SortXcelHLS.h
//========================================================================
// Header for testing the accelerator with a pure C++ flow.

#ifndef EX_SORT_SORT_XCEL_HLS_H
#define EX_SORT_SORT_XCEL_HLS_H

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

#include "mem/MemCommon.h"
#include "mem/MemMsg.h"
#include "mem/ArrayMemPortAdapter.h"

#include "xcel/XcelMsg.h"
#include "xcel/XcelWrapper.h"

void SortXcelHLS
(
  hls::stream<xcel::XcelReqMsg>&   xcelreq,
  hls::stream<xcel::XcelRespMsg>&  xcelresp,
  mem::MemReqStream&               memreq,
  mem::MemRespStream&              memresp
);

#endif /* EX_SORT_SORT_XCEL_HLS_H */

