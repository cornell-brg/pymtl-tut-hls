//========================================================================
// GcdXcelHLS.h
//========================================================================

#include <ap_utils.h>
#include <hls_stream.h>

#include "../xcel/XcelMsg.h"
#include "../xcel/XcelWrapper.h"

void GcdXcelHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp
);
