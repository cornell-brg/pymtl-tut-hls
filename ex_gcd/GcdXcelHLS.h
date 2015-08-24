//========================================================================
// GcdXcelHLS.h
//========================================================================

#ifndef EX_GCD_GCD_XCEL_HLS_H
#define EX_GCD_GCD_XCEL_HLS_H

#include <ap_utils.h>
#include <hls_stream.h>

#include "xcel/XcelMsg.h"
#include "xcel/XcelWrapper.h"

void GcdXcelHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp
);

#endif /* EX_GCD_GCD_XCEL_HLS_H */

