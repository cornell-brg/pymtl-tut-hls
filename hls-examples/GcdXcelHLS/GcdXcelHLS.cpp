//========================================================================
// GcdXcelHLS.cpp
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 10, 2015
//
// C++ implementation for the GcdXcel that uses the mtx/mfx interface.
// Acclerator registers are the following:
//
//  xr0 : go/result
//  xr1 : operand A
//  xr2 : operand B
//
// Accelerator protocol involves the following steps:
//
//  1. Write the operand A by writing to xr1
//  2. Write the operand B by writing to xr2
//  3. Tell accelerator to go by writing xr0
//  4. Wait for accelerator to finish by reading xr0, return result of gcd

#include "../common/interfaces.h"

//------------------------------------------------------------------------
// gcd
//------------------------------------------------------------------------

ap_uint<32> gcd( ap_uint<32> opA, ap_uint<32> opB ) {
#pragma HLS INLINE

  while ( opA != opB ) {
#pragma HLS PIPELINE
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

void GcdXcelHLS( hls::stream<XcelReqMsg>& xcelreq,
                 hls::stream<XcelRespMsg>& xcelresp )
{

  // Local variables
  XcelReqMsg  req;

  //  1. Write the operand A by writing to xr1
  req = xcelreq.read();
  ap_uint<32> opA = req.data;
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  //  2. Write the operand B by writing to xr2
  xcelreq.read( req );
  ap_uint<32> opB = req.data;
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  //  3. Tell accelerator to go by writing xr0
  xcelreq.read( req );
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  // Compute
  ap_uint<32> result = gcd( opA, opB );

  //  4. Wait for accelerator to finish by reading xr0, return result of gcd
  xcelreq.read( req );
  xcelresp.write( XcelRespMsg( req.id, result, req.type, req.opq ) );

}
