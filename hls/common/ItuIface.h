#ifndef ITUIFACE_H
#define ITUIFACE_H

#include "ap_int.h"
      
typedef ap_uint<97> ItuReqType;
typedef ap_uint<33> ItuRespType;
//typedef unsigned ItuReqType;
//typedef unsigned ItuRespType;

typedef struct _ItuIfaceType {
  ItuReqType  req;
  ItuRespType resp;

  _ItuIfaceType() : req(0), resp(0) {}
} ItuIfaceType;


#endif
