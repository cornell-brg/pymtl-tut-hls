//========================================================================
// interfaces.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 10, 2015
//
// Header file that defines the structs used for the memory and accelerator
// interfaces.

#ifndef INTERFACES_H
#define INTERFACES_H

#include <ap_int.h>
#include <hls_stream.h>

//------------------------------------------------------------------------
// XcelReqMsg
//------------------------------------------------------------------------

struct XcelReqMsg {
  ap_uint<11> id;
  ap_uint<32> data;
  ap_uint<5>  addr;
  ap_uint<1>  type;
  ap_uint<8>  opq;

  XcelReqMsg()
  : id( 0 ), data( 0 ), addr( 0 ),
    type( 0 ), opq( 0 ) {}

  XcelReqMsg( ap_uint<11> id_, ap_uint<32> data_,
    ap_uint<5> addr_, ap_uint<1> type_, ap_uint<8> opq_ )
  : id( id_ ), data( data_ ), addr( addr_ ),
    type( type_ ), opq( opq_ ) {}

};

//------------------------------------------------------------------------
// XcelRespMsg
//------------------------------------------------------------------------

struct XcelRespMsg {
  ap_uint<11> id;
  ap_uint<32> data;
  ap_uint<1>  type;
  ap_uint<8>  opq;

  XcelRespMsg()
  : id( 0 ), data( 0 ),
    type( 0 ), opq( 0 ) {}

  XcelRespMsg( ap_uint<11> id_, ap_uint<32> data_,
    ap_uint<1> type_, ap_uint<8> opq_ )
  : id( id_ ), data( data_ ),
    type( type_ ), opq( opq_ ) {}

};

//------------------------------------------------------------------------
// MemReqMsg
//------------------------------------------------------------------------

struct MemReqMsg {
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

//------------------------------------------------------------------------
// MemRespMsg
//------------------------------------------------------------------------

struct MemRespMsg {
  ap_uint<32> data;
  ap_uint<2>  len;
  ap_uint<8>  opq;
  ap_uint<3>  type;

  MemRespMsg()
  : data( 0 ), len( 0 ),
    opq( 0 ), type( 0 ) {}

  MemRespMsg( ap_uint<32> data_, ap_uint<2> len_,
    ap_uint<8> opq_, ap_uint<3> type_ )
  : data( data_ ), len( len_ ),
    opq( opq_ ), type( type_ ) {}

};

//------------------------------------------------------------------------
// IteratorReqMsg
//------------------------------------------------------------------------

struct IteratorReqMsg {
  ap_uint<32> data;
  ap_uint<32> addr;
  ap_uint<22> iter;
  ap_uint<4>  opc;
  ap_uint<11> ds_id;
  ap_uint<8>  opq;
};

//------------------------------------------------------------------------
// IteratorRespMsg
//------------------------------------------------------------------------

struct IteratorRespMsg {
  ap_uint<32> data;
  ap_uint<32> addr;
  ap_uint<4>  opc;
  ap_uint<11> ds_id;
  ap_uint<8>  opq;

  IteratorRespMsg() : data(0), addr(0), opc(0), ds_id(0), opq(0) {}

  IteratorRespMsg( ap_uint<32> data_, ap_uint<32> addr_, ap_uint<4> opc_,
    ap_uint<11> id_, ap_uint<8> opq_ )
  : data(data_), addr(addr_), opc(opc_), ds_id(id_), opq(opq_) {}

};

#endif
