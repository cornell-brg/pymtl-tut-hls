#ifndef COMMON_H
#define COMMON_H

#ifdef DEBUG
  #define DB_PRINT(x) printf x
  #define DB_ASSERT(x) assert x
#else
  #define DB_PRINT(x)
  #define DB_ASSERT(x)
#endif

#ifdef CPP_COMPILE
  static unsigned PTR_SIZE = sizeof(void*);
  typedef unsigned char*  Address;
  typedef Address*        AddressPtr;
#else
  #include <ap_int.h>

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
