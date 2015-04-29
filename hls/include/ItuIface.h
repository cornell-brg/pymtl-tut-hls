#ifndef ITUIFACE_H
#define ITUIFACE_H

#include "ap_int.h"
      
typedef ap_uint<97> ItuReqType;
typedef ap_uint<33> ItuRespType;
//typedef unsigned ItuReqType;
//typedef unsigned ItuRespType;

// Iface abstraction
typedef struct _ItuIfaceType {
  ItuReqType  req;
  ItuRespType resp;

  _ItuIfaceType() : req(0), resp(0) {}
} ItuIfaceType;

// helpers to read and write using Iface
inline 
ItuRespType itu_read (
    volatile ItuIfaceType &Iface,
    int id,
    int index
)
{
    ItuReqType req = 0;
    req |= id;
    req = (req << 32) | index;
    req = (req << 32);
    Iface.req = req;
    ap_wait();

    ItuRespType resp = Iface.resp;
    return resp;
}

inline 
ItuRespType itu_write (
    volatile ItuIfaceType &Iface,
    int id,
    int index,
    int data
)
{
    ItuReqType req = 1;
    req = (req << 32) | id;
    req = (req << 32) | index;
    req = (req << 32) | data;
    Iface.req = req;
    ap_wait();

    ItuRespType resp = Iface.resp;
    return resp;
}

#endif
