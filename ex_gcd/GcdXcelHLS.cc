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
//  3. Tell the accelerator to compute gcd and wait for result by reading
//     xr0

#include "ex_gcd/GcdXcelHLS.h"

using namespace xcel;

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

void GcdXcelHLS(
  hls::stream<xcel::XcelReqMsg>&  xcelreq,
  hls::stream<xcel::XcelRespMsg>& xcelresp
)
{

  XcelWrapper<3> xcelWrapper( xcelreq, xcelresp );

  // configure
  xcelWrapper.configure();

  // compute
  ap_uint<32> result = gcd( xcelWrapper.get_xreg(1), xcelWrapper.get_xreg(2) );

  // signal done
  xcelWrapper.done( result );

}
