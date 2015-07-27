#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#ifdef CPP_COMPILE
  typedef unsigned char*  Address;
  typedef Address*        AddressPtr;
#else
  typedef ap_uint<32>     Address;
  typedef Address         AddressPtr;
  
  #define MSG_READ  0
  #define MSG_WRITE 1

  #ifdef VIVADO_HLS_TEST  
    #include "../TestMem.h"
    extern TestMem& memreq;
    extern TestMem& memresp;
  #else
    #include "../../itu_ptrs/interfaces.h"
    extern hls::stream<MemReqMsg>  memreq;
    extern hls::stream<MemRespMsg> memresp;
  #endif

#endif  // CPP_COMPILE

#endif
