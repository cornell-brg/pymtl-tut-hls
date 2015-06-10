//========================================================================
// GcdXcelHLS.cpp
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 10, 2015
//
// C++ implementation for the GcdXcel that uses the mtx/mfx interface

#include "../common/interfaces.h"

//------------------------------------------------------------------------
// gcd
//------------------------------------------------------------------------

ap_uint<32> gcd( ap_uint<32> opA, ap_uint<32> opB ) {
  while ( opA != opB ) {
    if ( opA > opB )
      opA = opA - opB;
    else
      opB = opB - opA;
  }
  return opA;
}

//------------------------------------------------------------------------
// GcdXcelHLS
//------------------------------------------------------------------------

void GcdXcelHLS( hls::stream<XcelReqMsg>& xcelIn, hls::stream<XcelRespMsg>& xcelOut ) {
#pragma HLS pipeline II=1 enable_flush

  static ap_uint<32> opA    = 0;
  static ap_uint<32> opB    = 0;
  static ap_uint<32> result = 0;

  static enum { CONFIGURE, COMPUTE } gcdState = CONFIGURE;

  XcelReqMsg  req;
  XcelRespMsg resp;

  switch( gcdState ) {

    case CONFIGURE:
    {
      if ( !xcelIn.empty() ) {
        xcelIn.read( req );

        switch ( req.addr ) {
          case 0:
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelOut.write( resp );
            gcdState  = COMPUTE;
            break;

          case 1:
            opA = req.data;
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelOut.write( resp );
            gcdState  = CONFIGURE;
            break;

          case 2:
            opB = req.data;
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelOut.write( resp );
            gcdState  = CONFIGURE;
            break;
        }
      }
      break;
    }

    case COMPUTE:
    {
      result = gcd( opA, opB );
      if ( !xcelIn.empty() ) {
        xcelIn.read( req );
        resp = XcelRespMsg( req.id, result, req.type, req.opq );
        xcelOut.write( resp );
        gcdState = CONFIGURE;
      }
      break;
    }

  }

}
