#ifndef INTERFACES_H
#define INTERFACES_H

#include "ap_int.h"
#include "hls_stream.h"

#define MSG_READ 0
#define MSG_WRITE 1

//------------------------------------------------------------------------
// Accel Iface Types
//------------------------------------------------------------------------
typedef ap_uint<11> AcIdType;
typedef ap_uint<32> AcDataType;
typedef ap_uint<5>  AcAddrType;
typedef ap_uint<1>  AcRwType;
typedef ap_uint<8>  AcOpqType;

//------------------------------------------------------------------------
// AcReqMsg
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
// AcRespMsg
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
// Accel Iface
//------------------------------------------------------------------------
typedef struct _AcIfaceType {
  hls::stream<AcReqMsg>  req;
  hls::stream<AcRespMsg> resp;
} AcIfaceType;

//------------------------------------------------------------------------
// Dtu Iface Types
//------------------------------------------------------------------------
/*typedef ap_uint<32> DtuDataType;
typedef ap_uint<32> DtuFieldType;
typedef ap_uint<22> DtuIterType;
typedef ap_uint<11> DtuIdType;
typedef ap_uint<1>  DtuRwType;
typedef ap_uint<8>  DtuOpqType;

//------------------------------------------------------------------------
// DtuReqMsg
//------------------------------------------------------------------------

struct DtuReqMsg {
  DtuDataType  data;
  DtuFieldType field;
  DtuIterType  iter;
  DtuIdType    id;
  DtuRwType    type;
  DtuOpqType   opq;

  DtuReqMsg()
  : data( 0 ), field( 0 ), iter( 0 ), id( 0 ),
    type( 0 ), opq( 0 ) {}

  DtuReqMsg( ap_uint<32> data_, ap_uint<32> field_, ap_uint<22> iter_,
    ap_uint<11> id_, ap_uint<1> type_, ap_uint<8> opq_ )
  : data( data_ ), field( field_ ), iter( iter_ ), id( id_ ),
    type( type_ ), opq( opq_ ) {}
};

//------------------------------------------------------------------------
// DtuRespMsg
//------------------------------------------------------------------------

struct DtuRespMsg {
  DtuIdType   id;
  DtuDataType data;
  DtuRwType   type;
  DtuOpqType  opq;

  DtuRespMsg()
  : id( 0 ), data( 0 ), type( 0 ), opq( 0 ) {}

  DtuRespMsg( ap_uint<11> id_, ap_uint<32> data_,
    ap_uint<1> type_, ap_uint<8> opq_ )
  : id( id_ ), data( data_ ), type( type_ ), opq( opq_ ) {}
};
*/

//------------------------------------------------------------------------
// Mem Iface Types
//------------------------------------------------------------------------
typedef ap_uint<32> MemDataType;
typedef ap_uint<2>  MemLenType;
typedef ap_uint<32> MemAddrType;
typedef ap_uint<8>  MemOpqType;
typedef ap_uint<3>  MemRwType;

//------------------------------------------------------------------------
// MemReqMsg
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
// MemRespMsg
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
// Mem Iface
//------------------------------------------------------------------------
typedef struct _MemIfaceType {
  hls::stream<MemReqMsg>  req;
  hls::stream<MemRespMsg> resp;
} MemIfaceType;

MemRespMsg mem_read (MemIfaceType& iface,
                     MemAddrType addr, MemLenType len)
{
  #pragma HLS INLINE
  // note that a len of 0 means max number of bytes
  iface.req.write( MemReqMsg( 0, len, addr, 0, 0 ) );
  
  ap_wait();

  MemRespMsg resp = iface.resp.read();
  return resp;
}

MemRespMsg mem_write (MemIfaceType& iface, 
                      MemAddrType addr, MemLenType len, MemDataType data)
{
  #pragma HLS INLINE
  // note that a len of 0 means max number of bytes
  iface.req.write( MemReqMsg( data, len, addr, 0, 1 ) );
  
  ap_wait();

  MemRespMsg resp = iface.resp.read();
  return resp;
}

#endif
