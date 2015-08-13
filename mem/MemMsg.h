//========================================================================
// MemMsg.h
//========================================================================
// Header file that defines the structs used for the memory interfaces

#include <ap_int.h>

#ifndef MEM_MEMMSG_H
#define MEM_MEMMSG_H

namespace mem {

  //----------------------------------------------------------------------
  // MemReqMsg
  //----------------------------------------------------------------------

  struct MemReqMsg {

    // memory message types
    static const int TYPE_READ  = 0;
    static const int TYPE_WRITE = 1;

    // memory request
    ap_uint<32> data;
    ap_uint<2>  len;
    ap_uint<32> addr;
    ap_uint<8>  opq;
    ap_uint<3>  type;

    MemReqMsg()
    : data( 0 ), len( 0 ), addr( 0 ),
      opq( 0 ), type( 0 ) {}

    MemReqMsg( ap_uint<32> data_, ap_uint<2> len_,
      ap_uint<32> addr_, ap_uint<8> opq_, ap_uint<3> type_ )
    : data( data_ ), len( len_ ), addr( addr_ ),
      opq( opq_ ), type( type_ ) {}

  };

  //----------------------------------------------------------------------
  // MemRespMsg
  //----------------------------------------------------------------------

  struct MemRespMsg {
    ap_uint<32> data;
    ap_uint<2>  len;
    ap_uint<8>  opq;
    ap_uint<3>  type;

    static const int TYPE_READ  = 0;
    static const int TYPE_WRITE = 1;

    MemRespMsg()
    : data( 0 ), len( 0 ),
      opq( 0 ), type( 0 ) {}

    MemRespMsg( ap_uint<32> data_, ap_uint<2> len_,
      ap_uint<8> opq_, ap_uint<3> type_ )
    : data( data_ ), len( len_ ),
      opq( opq_ ), type( type_ ) {}

  };

}

#endif
