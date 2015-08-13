//========================================================================
// XcelMsg.h
//========================================================================
// Header file that defines the structs used for the accelerator interfaces.

#include <ap_int.h>

#ifndef XCEL_XCELMSG_H
#define XCEL_XCELMSG_H

namespace xcel {

  //----------------------------------------------------------------------
  // XcelReqMsg
  //----------------------------------------------------------------------

  struct XcelReqMsg {
    ap_uint<11> id;
    ap_uint<32> data;
    ap_uint<5>  addr;
    ap_uint<1>  type;
    ap_uint<8>  opq;

    static const int TYPE_READ  = 0;
    static const int TYPE_WRITE = 1;

    XcelReqMsg()
    : id( 0 ), data( 0 ), addr( 0 ),
      type( 0 ), opq( 0 ) {}

    XcelReqMsg( ap_uint<11> id_, ap_uint<32> data_,
      ap_uint<5> addr_, ap_uint<1> type_, ap_uint<8> opq_ )
    : id( id_ ), data( data_ ), addr( addr_ ),
      type( type_ ), opq( opq_ ) {}

  };

  //----------------------------------------------------------------------
  // XcelRespMsg
  //----------------------------------------------------------------------

  struct XcelRespMsg {
    ap_uint<11> id;
    ap_uint<32> data;
    ap_uint<1>  type;
    ap_uint<8>  opq;

    static const int TYPE_READ  = 0;
    static const int TYPE_WRITE = 1;

    XcelRespMsg()
    : id( 0 ), data( 0 ),
      type( 0 ), opq( 0 ) {}

    XcelRespMsg( ap_uint<11> id_, ap_uint<32> data_,
      ap_uint<1> type_, ap_uint<8> opq_ )
    : id( id_ ), data( data_ ),
      type( type_ ), opq( opq_ ) {}

  };

}
#endif
