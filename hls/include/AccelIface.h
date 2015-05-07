#ifndef ACCEL_IFACE_H
#define ACCEL_IFACE_H

#include "ap_int.h"

#define AC_OP_BITS 8
#define AC_TYPE_BITS 1
#define AC_ADDR_BITS 5
#define AC_DATA_BITS 32
#define AC_ID_BITS 11
#define AC_REQ_BITS (AC_OP_BITS + AC_TYPE_BITS + AC_ADDR_BITS + \
    AC_DATA_BITS + AC_ID_BITS)
#define AC_RESP_BITS (AC_OP_BITS + AC_TYPE_BITS + AC_DATA_BITS + AC_ID_BITS)

typedef ap_uint<AC_REQ_BITS>  AcReqType;
typedef ap_uint<AC_RESP_BITS> AcRespType;
typedef ap_uint<AC_OP_BITS> AcOpaqueType;
typedef ap_uint<AC_TYPE_BITS> AcRWType;
typedef ap_uint<AC_ID_BITS> AcIdType;
typedef ap_uint<AC_DATA_BITS> AcDataType;
typedef ap_uint<AC_ADDR_BITS> AcAddrType;

AcReqType make_req (
    AcIdType id, AcDataType data, AcAddrType addr, 
    AcRWType rw, AcOpaqueType op=0
)
{
  AcReqType req = 0;
  req |= op;
  req = (req << AC_TYPE_BITS) | rw;
  req = (req << AC_ADDR_BITS) | addr;
  req = (req << AC_DATA_BITS) | data;
  req = (req << AC_ID_BITS) | id;
  return req;
}

AcRespType make_resp (
    AcIdType id, AcDataType data, 
    AcRWType rw, AcOpaqueType op=0
)
{
  AcRespType resp = 0;
  resp |= op;
  resp = (resp << AC_TYPE_BITS) | rw;
  resp = (resp << AC_DATA_BITS) | data;
  resp = (resp << AC_ID_BITS) | id;
  return resp;
}

#define AC_REQ_TYPE(x) ((x).range(AC_TYPE_BITS + AC_ADDR_BITS + AC_DATA_BITS + AC_ID_BITS - 1, \
             AC_ADDR_BITS + AC_DATA_BITS + AC_ID_BITS))
#define AC_REQ_ADDR(x) ((x).range(AC_ADDR_BITS + AC_DATA_BITS + AC_ID_BITS - 1, AC_DATA_BITS + AC_ID_BITS))
#define AC_REQ_DATA(x) ((x).range(AC_DATA_BITS + AC_ID_BITS - 1, AC_ID_BITS))
#define AC_REQ_ID(x) ((x).range(AC_ID_BITS - 1, 0))

#define AC_RESP_TYPE(x) ((x).range(AC_TYPE_BITS + AC_DATA_BITS + AC_ID_BITS - 1, AC_DATA_BITS + AC_ID_BITS))
#define AC_RESP_DATA(x) AC_REQ_DATA(x)
#define AC_RESP_ID(x) AC_REQ_ID(x)


#endif
