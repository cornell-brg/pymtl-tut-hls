//========================================================================
// MemCommon.h
//========================================================================

#ifndef MEM_MEM_COMMON_H
#define MEM_MEM_COMMON_H

#include <hls_stream.h>

#include "mem/TestMem.h"
#include "mem/MemMsg.h"

namespace mem {

  #ifdef XILINX_VIVADO_HLS_TESTING
  typedef mem::TestMem  MemReqStream;
  typedef mem::TestMem  MemRespStream;
  #else
  typedef hls::stream<MemReqMsg<> >  MemReqStream;
  typedef hls::stream<MemRespMsg<> > MemRespStream;
  #endif
  
  //XXX:This is only true for HLS or HLS_TESTING.
  //    For C++ compilation, PTR_SIZE should be sizeof(void*)
  //    which is 8 on brg
  static unsigned PTR_SIZE = 4;

  typedef unsigned        Address;

}

#endif /* MEM_MEM_COMMON_H */

