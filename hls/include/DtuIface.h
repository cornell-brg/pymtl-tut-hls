#ifndef DTU_IFACE_H
#define DTU_IFACE_H

#include "ap_int.h"

#define DTU_TYPE_BITS 1
#define DTU_DSID_BITS 11
#define DTU_ITER_BITS 22
#define DTU_FIELD_BITS 32
#define DTU_DATA_BITS 32
#define DTU_REQ_BITS (DTU_TYPE_BITS + DTU_DSID_BITS + \
    DTU_ITER_BITS + DTU_FIELD_BITS + DTU_DATA_BITS)
#define DTU_RESP_BITS (DTU_TYPE_BITS+DTU_DATA_BITS)
      
typedef ap_uint<DTU_REQ_BITS>  DtuReqType;
typedef ap_uint<DTU_RESP_BITS> DtuRespType;
typedef ap_uint<DTU_DATA_BITS> DtuDataType;

#define DTU_RESP_DATA(resp) ((resp).range(DTU_DATA_BITS-1,0))

// Iface abstraction
typedef struct _DtuIfaceType {
  DtuReqType  req;
  DtuRespType resp;

  _DtuIfaceType() : req(0), resp(0) {}
} DtuIfaceType;

// ########################################################
// functions to read and write using Iface
// ########################################################
inline 
DtuRespType dtu_read (
    volatile DtuIfaceType &Iface,
    ap_uint<DTU_DSID_BITS> dsid,
    ap_uint<DTU_ITER_BITS> iter,
    ap_uint<DTU_FIELD_BITS> field=0
)
{
    DtuReqType req = 0;
    req = (req << DTU_DSID_BITS) | dsid;
    req = (req << DTU_ITER_BITS) | iter;
    req = (req << DTU_FIELD_BITS) | field;
    req = (req << DTU_DATA_BITS);
    Iface.req = req;
    ap_wait();

    DtuRespType resp = Iface.resp;
    return resp;
}

inline 
DtuRespType dtu_write_field (
    volatile DtuIfaceType &Iface,
    ap_uint<DTU_DSID_BITS> dsid,
    ap_uint<DTU_ITER_BITS> iter,
    ap_uint<DTU_FIELD_BITS> field,
    ap_uint<DTU_DATA_BITS> data
)
{
    DtuReqType req = 1;
    req = (req << DTU_DSID_BITS) | dsid;
    req = (req << DTU_ITER_BITS) | iter;
    req = (req << DTU_FIELD_BITS) | field;
    req = (req << DTU_DATA_BITS) | data;
    Iface.req = req;
    ap_wait();

    DtuRespType resp = Iface.resp;
    return resp;
}

inline 
DtuRespType dtu_write (
    volatile DtuIfaceType &Iface,
    ap_uint<DTU_DSID_BITS> dsid,
    ap_uint<DTU_ITER_BITS> iter,
    ap_uint<DTU_DATA_BITS> data
)
{
  return dtu_write_field( Iface, dsid, iter, 0, data );
}

#endif
