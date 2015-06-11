//========================================================================
// VecincrXcelHLS.cpp
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 10, 2015
//
// C++ implementation for the VecincrXcel. Increments all elements in a
// vector stored in memory Accelerator register interface:
//
//  xr0 : go/done
//  xr1 : base address of the array
//  xr2 : size of the array
//  xr3 : value to increment by
//
// Accelerator protocol involves the following steps:
//  1. Write the base address of the array to xr1
//  2. Write the number of elements in the array to xr2
//  3. Write the value to increment the array elemments to xr3
//  3. Tell accelerator to go by writing xr0
//  4. Wait for accelerator to finish by reading xr0, result will be 1

#include "../common/interfaces.h"
#include <ap_utils.h>

//------------------------------------------------------------------------
// vecincr
//------------------------------------------------------------------------

#define READ  0
#define WRITE 1

void vecincr( ap_uint<32> base,
              ap_uint<32> size,
              ap_uint<32> incr,
              hls::stream<MemReqMsg>&  memreq,
              hls::stream<MemRespMsg>& memresp
            )
{
#pragma HLS INLINE

  MemRespMsg resp;

  ap_uint<32> temp;

  for ( int i = 0; i < size; ++ i ) {
    // memory read request
    memreq.write( MemReqMsg( 0, 0, (base + i*4), 0, READ ) );
    ap_wait();

    // memory read response
    memresp.read( resp );
    ap_wait();

    // increment the element
    temp = resp.data + incr;

    // memory write request
    memreq.write( MemReqMsg( temp, 0, (base + i*4), 0, WRITE ) );
    ap_wait();

    // memory write response
    memresp.read();
  }

}

//------------------------------------------------------------------------
// VecincrXcelHLS
//------------------------------------------------------------------------

void VecincrXcelHLS( hls::stream<XcelReqMsg>&  xcelreq,
                     hls::stream<XcelRespMsg>& xcelresp,
                     hls::stream<MemReqMsg>&   memreq,
                     hls::stream<MemRespMsg>&  memresp
                   )
{

  static ap_uint<32> base = 0;
  static ap_uint<32> size = 0;
  static ap_uint<32> incr = 0;

  static enum { CONFIGURE, COMPUTE } vecincrState = CONFIGURE;

  XcelReqMsg  req;
  XcelRespMsg resp;

  switch( vecincrState ) {

    case CONFIGURE:
    {
      if ( !xcelreq.empty() ) {
        xcelreq.read( req );

        switch ( req.addr ) {
          case 0:
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelresp.write( resp );
            vecincrState  = COMPUTE;
            break;

          case 1:
            base = req.data;
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelresp.write( resp );
            vecincrState  = CONFIGURE;
            break;

          case 2:
            size = req.data;
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelresp.write( resp );
            vecincrState  = CONFIGURE;
            break;

          case 3:
            incr = req.data;
            resp = XcelRespMsg( req.id, 0, req.type, req.opq );
            xcelresp.write( resp );
            vecincrState  = CONFIGURE;
            break;
        }
      }
      break;
    }

    case COMPUTE:
    {

      vecincr( base, size, incr, memreq, memresp );

      if ( !xcelreq.empty() ) {
        xcelreq.read( req );
        resp = XcelRespMsg( req.id, 1, req.type, req.opq );
        xcelresp.write( resp );
        vecincrState = CONFIGURE;
      }
      break;
    }

  }

}
