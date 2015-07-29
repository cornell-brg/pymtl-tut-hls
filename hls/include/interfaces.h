#ifndef INTERFACES_H
#define INTERFACES_H

#include "ap_int.h"
#include "hls_stream.h"

#define MSG_READ 0
#define MSG_WRITE 1

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// Accel Iface
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef ap_uint<11> AcIdType;
typedef ap_uint<32> AcDataType;
typedef ap_uint<5>  AcAddrType;
typedef ap_uint<1>  AcRwType;
typedef ap_uint<8>  AcOpqType;

//------------------------------------------------------------------------

struct AcReqMsg {
  AcIdType   id;
  AcDataType data;
  AcAddrType addr;
  AcRwType   type;
  AcOpqType  opq;

  AcReqMsg()
  : id( 0 ), data( 0 ), addr( 0 ), type( 0 ), opq( 0 ) {}

  AcReqMsg( AcIdType id_, AcDataType data_,
    AcAddrType addr_, AcRwType type_, AcOpqType opq_ )
  : id( id_ ), data( data_ ), addr( addr_ ), type( type_ ), opq( opq_ ) {}
};

//------------------------------------------------------------------------

struct AcRespMsg {
  AcIdType   id;
  AcDataType data;
  AcRwType   type;
  AcOpqType  opq;

  AcRespMsg()
  : id( 0 ), data( 0 ), type( 0 ), opq( 0 ) {}

  AcRespMsg( AcIdType id_, AcDataType data_,
    AcRwType type_, AcOpqType opq_ )
  : id( id_ ), data( data_ ), type( type_ ), opq( opq_ ) {}
};

//------------------------------------------------------------------------

typedef struct _AcIfaceType {
  hls::stream<AcReqMsg>  req;
  hls::stream<AcRespMsg> resp;
} AcIfaceType;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// Dstu Iface
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef ap_uint<32> DstuDataType;
typedef ap_uint<32> DstuFieldType;
typedef ap_uint<22> DstuIterType;
typedef ap_uint<11> DstuIdType;
typedef ap_uint<1>  DstuRwType;
typedef ap_uint<8>  DstuOpqType;

//------------------------------------------------------------------------

struct DstuReqMsg {
  DstuDataType  data;
  DstuFieldType field;
  DstuIterType  iter;
  DstuIdType    id;
  DstuRwType    type;
  DstuOpqType   opq;

  DstuReqMsg()
  : data( 0 ), field( 0 ), iter( 0 ), id( 0 ),
    type( 0 ), opq( 0 ) {}

  DstuReqMsg( DstuDataType data_, DstuFieldType field_, DstuIterType iter_,
    DstuIdType id_, DstuRwType type_, DstuOpqType opq_ )
  : data( data_ ), field( field_ ), iter( iter_ ), id( id_ ),
    type( type_ ), opq( opq_ ) {}
};

//------------------------------------------------------------------------

struct DstuRespMsg {
  DstuIdType   id;
  DstuDataType data;
  DstuRwType   type;
  DstuOpqType  opq;

  DstuRespMsg()
  : id( 0 ), data( 0 ), type( 0 ), opq( 0 ) {}

  DstuRespMsg( DstuIdType id_, DstuDataType data_,
    DstuRwType type_, DstuOpqType opq_ )
  : id( id_ ), data( data_ ), type( type_ ), opq( opq_ ) {}
};

//------------------------------------------------------------------------

typedef struct _DstuIfaceType {
  hls::stream<DstuReqMsg>  req;
  hls::stream<DstuRespMsg> resp;
} DstuIfaceType;

//------------------------------------------------------------------------

DstuRespMsg dstu_read (
    DstuIfaceType &iface,
    DstuIdType id, DstuIterType iter,
    DstuFieldType field)
{
  #pragma HLS INLINE
  iface.req.write( DstuReqMsg( 0, field, iter, id, MSG_READ, 0 ) );
  ap_wait();
  DstuRespMsg resp = iface.resp.read();
  return resp;
}

DstuRespMsg dstu_write (
    DstuIfaceType &iface,
    DstuIdType id, DstuIterType iter, 
    DstuFieldType field, DstuDataType data)
{
  #pragma HLS INLINE
  iface.req.write( DstuReqMsg( data, field, iter, id, MSG_WRITE, 0 ) );
  ap_wait();
  DstuRespMsg resp = iface.resp.read();
  return resp;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// PtrItu Iface
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define OPC_READ  0
#define OPC_WRITE 1
#define OPC_INCR  2
#define OPC_DECR  3

typedef ap_uint<8>  PtrItuOpqType;
typedef ap_uint<11> PtrItuIdType;
typedef ap_uint<4>  PtrItuOpType;
typedef ap_uint<22> PtrItuIterType;
typedef ap_uint<32> PtrItuAddrType;
typedef ap_uint<32> PtrItuDataType;

//------------------------------------------------------------------------

struct PtrItuReqMsg {
  PtrItuOpqType   opq;
  PtrItuIdType    id;
  PtrItuOpType    opcode;
  PtrItuIterType  iter;
  PtrItuAddrType  addr;
  PtrItuDataType  data;

  PtrItuReqMsg()
  : opq( 0 ), id( 0 ), opcode( 0 ), iter( 0 ), addr( 0 ), data( 0 ) {}

  PtrItuReqMsg( PtrItuOpqType opq_, PtrItuIdType id_, PtrItuOpType opcode_,
    PtrItuIterType iter_, PtrItuAddrType addr_, PtrItuDataType data_ )
  : opq( opq_ ), id( id_ ), opcode( opcode_ ), iter( iter_ ), 
    addr( addr_ ), data( data_ ) {}
};

//------------------------------------------------------------------------

struct PtrItuRespMsg {
  PtrItuOpqType  opq;
  PtrItuIdType   id;
  PtrItuOpType   opcode;
  PtrItuAddrType addr;
  PtrItuDataType data;

  PtrItuRespMsg()
  : opq( 0 ), id( 0 ), opcode( 0 ), addr( 0 ), data( 0 ) {}

  PtrItuRespMsg( PtrItuOpqType opq_, PtrItuIdType id_, PtrItuOpType opcode_,
      PtrItuAddrType addr_, PtrItuDataType data_ )
  : opq( opq_ ), id( id_ ), opcode( opcode_ ), addr( addr_ ), data( data_ ) {}
};

//------------------------------------------------------------------------

typedef struct _PtrItuIfaceType {
  hls::stream<PtrItuReqMsg>  req;
  hls::stream<PtrItuRespMsg> resp;
} PtrItuIfaceType;

//------------------------------------------------------------------------

PtrItuRespMsg ptr_itu_read (
    PtrItuIfaceType &iface,
    PtrItuIdType id, PtrItuIterType iter,
    PtrItuAddrType addr)
{
  #pragma HLS INLINE
  iface.req.write( PtrItuReqMsg( 0, id, OPC_READ, iter, addr, 0 ) );
  ap_wait();
  PtrItuRespMsg resp = iface.resp.read();
  return resp;
}

PtrItuRespMsg ptr_itu_write (
    PtrItuIfaceType &iface,
    PtrItuIdType id, PtrItuIterType iter, 
    PtrItuAddrType addr, PtrItuDataType data)
{
  #pragma HLS INLINE
  iface.req.write( PtrItuReqMsg( 0, id, OPC_WRITE, iter, addr, data ) );
  ap_wait();
  PtrItuRespMsg resp = iface.resp.read();
  return resp;
}

PtrItuRespMsg ptr_itu_incr (
    PtrItuIfaceType &iface,
    PtrItuIdType id, PtrItuIterType iter, 
    PtrItuAddrType addr)
{
  #pragma HLS INLINE
  iface.req.write( PtrItuReqMsg( 0, id, OPC_INCR, iter, addr, 0 ) );
  ap_wait();
  PtrItuRespMsg resp = iface.resp.read();
  return resp;
}

PtrItuRespMsg ptr_itu_decr (
    PtrItuIfaceType &iface,
    PtrItuIdType id, PtrItuIterType iter, 
    PtrItuAddrType addr)
{
  #pragma HLS INLINE
  iface.req.write( PtrItuReqMsg( 0, id, OPC_DECR, iter, addr, 0 ) );
  ap_wait();
  PtrItuRespMsg resp = iface.resp.read();
  return resp;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// Mem Iface
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef ap_uint<32> MemDataType;
typedef ap_uint<2>  MemLenType;
typedef ap_uint<32> MemAddrType;
typedef ap_uint<8>  MemOpqType;
typedef ap_uint<3>  MemRwType;

//------------------------------------------------------------------------

struct MemReqMsg {
  MemDataType data;
  MemLenType  len;
  MemAddrType addr;
  MemOpqType  opq;
  MemRwType   type;

  MemReqMsg()
  : data( 0 ), len( 0 ), addr( 0 ), opq( 0 ), type( 0 ) {}

  MemReqMsg( MemDataType data_, MemLenType len_,
    MemAddrType addr_, MemOpqType opq_, MemRwType type_ )
  : data( data_ ), len( len_ ), addr( addr_ ), opq( opq_ ), type( type_ ) {}
};

//------------------------------------------------------------------------

struct MemRespMsg {
  MemDataType data;
  MemLenType  len;
  MemOpqType  opq;
  MemRwType   type;

  MemRespMsg()
  : data( 0 ), len( 0 ), opq( 0 ), type( 0 ) {}

  MemRespMsg( MemDataType data_, MemLenType len_,
    MemOpqType opq_, MemRwType type_ )
  : data( data_ ), len( len_ ), opq( opq_ ), type( type_ ) {}
};

//------------------------------------------------------------------------

typedef struct _MemIfaceType {
  hls::stream<MemReqMsg>  req;
  hls::stream<MemRespMsg> resp;
} MemIfaceType;

//------------------------------------------------------------------------

MemRespMsg mem_read (
    MemIfaceType& iface,
    MemAddrType addr, MemLenType len)
{
  #pragma HLS INLINE
  // note that a len of 0 means max number of bytes
  iface.req.write( MemReqMsg( 0, len, addr, 0, MSG_READ ) );
  ap_wait();
  MemRespMsg resp = iface.resp.read();
  return resp;
}

MemRespMsg mem_write (
    MemIfaceType& iface, 
    MemAddrType addr, MemLenType len, MemDataType data)
{
  #pragma HLS INLINE
  // note that a len of 0 means max number of bytes
  iface.req.write( MemReqMsg( data, len, addr, 0, MSG_WRITE ) );
  ap_wait();
  MemRespMsg resp = iface.resp.read();
  return resp;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// Lane Unit Iface
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef ap_uint<3>  PeIdType;
typedef ap_uint<1>  PeRwType;
typedef ap_uint<32> PeDataType;

//------------------------------------------------------------------------

struct PeReqMsg {
  PeIdType   id;
  PeRwType   type;
  PeDataType data;

  PeReqMsg()
  : id( 0 ), type( 0 ), data( 0 ) {}

  PeReqMsg( PeIdType id_, PeRwType type_, PeDataType data_)
  : id( id_ ), type( type_ ), data( data_ ) {}
};

//------------------------------------------------------------------------

struct PeRespMsg {
  PeIdType   id;
  PeRwType   type;
  PeDataType data;

  PeRespMsg()
  : id( 0 ), type( 0 ), data( 0 ) {}

  PeRespMsg( PeIdType id_, PeRwType type_, PeDataType data_)
  : id( id_ ), type( type_ ), data( data_ ) {}
};

//------------------------------------------------------------------------

typedef struct _PeIfaceType {
  hls::stream<PeReqMsg>  req;
  hls::stream<PeRespMsg> resp;
} PeIfaceType;

//------------------------------------------------------------------------

PeRespMsg pe_read (PeIfaceType& iface)
{
  #pragma HLS INLINE
  iface.req.write( PeReqMsg( 0, MSG_READ, 0 ) );
  ap_wait();
  PeRespMsg resp = iface.resp.read();
  return resp;
}

PeRespMsg pe_write (PeIfaceType& iface, PeDataType data)
{
  #pragma HLS INLINE
  iface.req.write( PeReqMsg( 0, MSG_WRITE, data ) );
  ap_wait();
  PeRespMsg resp = iface.resp.read();
  return resp;
}

#endif
