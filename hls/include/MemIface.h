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

#endif
