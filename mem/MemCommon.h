//========================================================================
// MemCommon.h
//========================================================================

#ifndef MEM_MEMCOMMON_H
#define MEM_MEMCOMMON_H

#include <hls_stream.h>

#include "../mem/TestMem.h"
#include "../mem/MemMsg.h"

namespace mem {

  #ifdef XILINX_VIVADO_HLS_TESTING
  typedef mem::TestMem  MemReqStream;
  typedef mem::TestMem  MemRespStream;
  #else
  typedef hls::stream<MemReqMsg<> >  MemReqStream;
  typedef hls::stream<MemRespMsg<> > MemRespStream;
  #endif

}

#endif
