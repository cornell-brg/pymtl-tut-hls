#ifndef LIST_COMMON_H
#define LIST_COMMON_H

  #include <ap_int.h>

#ifdef CPP_COMPILE
  static unsigned PTR_SIZE = sizeof(void*);
  typedef unsigned char*  Address;
  typedef Address*        AddressPtr;
#else
  static unsigned PTR_SIZE = 4;
  typedef ap_uint<32>     Address;
  typedef Address         AddressPtr;

  #define MSG_READ  0
  #define MSG_WRITE 1

  #include "../../itu_ptrs/interfaces.h"
  extern hls::stream<MemReqMsg>  memreq;
  extern hls::stream<MemRespMsg> memresp;

#endif  // CPP_COMPILE

#endif
