#ifndef MEM_IFACE_H
#define MEM_IFACE_H

#include "ap_int.h"

#define MEM_TYPE_BITS 3
#define MEM_OP_BITS 8
#define MEM_ADDR_BITS 32
#define MEM_LEN_BITS 2
#define MEM_DATA_BITS 32
#define MEM_REQ_BITS (MEM_TYPE_BITS + MEM_OP_BITS + \
    MEM_ADDR_BITS + MEM_LEN_BITS + MEM_DATA_BITS)
#define MEM_RESP_BITS (MEM_TYPE_BITS + MEM_OP_BITS + \
    MEM_LEN_BITS + MEM_DATA_BITS)

typedef ap_uint<MEM_REQ_BITS>  MemReqType;
typedef ap_uint<MEM_RESP_BITS> MemRespType;
typedef ap_uint<MEM_TYPE_BITS> MemRWType;
typedef ap_uint<MEM_OP_BITS>   MemOpType;
typedef ap_uint<MEM_ADDR_BITS> MemAddrType;
typedef ap_uint<MEM_LEN_BITS>  MemLenType;
typedef ap_uint<MEM_DATA_BITS> MemDataType;

//----------------------------------------------------------------------
// Bitfield access
//----------------------------------------------------------------------
#define MEM_REQ_TYPE(x) ((x).range(MEM_TYPE_BITS + MEM_OP_BITS + MEM_ADDR_BITS + MEM_LEN_BITS + \
      MEM_DATA_BITS, MEM_OP_BITS + MEM_ADDR_BITS + MEM_LEN_BITS + MEM_DATA_BITS))
#define MEM_REQ_OP(x) ((x).range(MEM_OP_BITS + MEM_ADDR_BITS + MEM_LEN_BITS + MEM_DATA_BITS, \
      MEM_ADDR_BITS + MEM_LEN_BITS + MEM_DATA_BITS))
#define MEM_REQ_ADDR(x) ((x).range(MEM_ADDR_BITS + MEM_LEN_BITS + MEM_DATA_BITS, \
      MEM_LEN_BITS + MEM_DATA_BITS))
#define MEM_REQ_LEN(x) ((x).range(MEM_LEN_BITS + MEM_DATA_BITS, MEM_DATA_BITS))
#define MEM_REQ_DATA(x) ((x).range(MEM_DATA_BITS, 0))

#define MEM_RESP_TYPE(x) ((x).range(MEM_TYPE_BITS + MEM_OP_BITS + MEM_LEN_BITS + \
      MEM_DATA_BITS, MEM_OP_BITS + MEM_LEN_BITS + MEM_DATA_BITS))
#define MEM_RESP_OP(x) ((x).range(MEM_OP_BITS + MEM_LEN_BITS + MEM_DATA_BITS, \
      MEM_LEN_BITS + MEM_DATA_BITS))
#define MEM_RESP_LEN(x) ((x).range(MEM_LEN_BITS + MEM_DATA_BITS, MEM_DATA_BITS))
#define MEM_RESP_DATA(x) ((x).range(MEM_DATA_BITS, 0))

//----------------------------------------------------------------------
// Memory Iface Abstraction
//----------------------------------------------------------------------
typedef struct _MemIfaceType {
  MemReqType req;
  MemRespType resp;
} MemIfaceType;

//----------------------------------------------------------------------
// Reading and Writing the iface
//----------------------------------------------------------------------
MemReqType make_mem_req (
    MemRWType rw, MemOpType op, MemAddrType addr, MemLenType len, MemDataType data
)
{
  MemReqType req = 0;
  req |= rw;
  req = (req << MEM_OP_BITS)   | op;
  req = (req << MEM_ADDR_BITS) | addr;
  req = (req << MEM_LEN_BITS)  | len;
  req = (req << MEM_DATA_BITS) | data;
  return req;
}

MemRespType mem_read (volatile MemIfaceType &iface,
                      MemAddrType addr, MemLenType len)
{
  MemReqType req = make_mem_req (0, 0, addr, len, 0);
  iface.req = req;
  
  ap_wait();

  MemReqType resp = iface.resp;
  return resp;
}

MemRespType mem_write (volatile MemIfaceType &iface, 
                       MemAddrType addr, MemLenType len, MemDataType data)
{
  MemReqType req = make_mem_req (1, 0, addr, len, data);
  iface.req = req;
  
  ap_wait();

  MemReqType resp = iface.resp;
  return resp;
}

//----------------------------------------------------------------------
// Read MetaData
//----------------------------------------------------------------------
#include "MetaData.h"

void mem_read_metadata (volatile MemIfaceType &iface, MemAddrType addr) {
  MemReqType req;
  MemRespType resp;

  // first read the word at addr
  resp = mem_read (iface, addr, 4);
  unsigned metadata0 = MEM_RESP_DATA(resp);

  // Check how many fields from the metadata.
  // zero means a primitive type, non-zero means a struct
  int fields = GET_FIELDS(metadata0);
  if (fields == 0) {
  }
  else {
  }
}

#endif
