//========================================================================
// MemCommon.h
//========================================================================

#ifndef XMEM_MEM_COMMON_H
#define XMEM_MEM_COMMON_H

#include <ap_int.h>
#include <hls_stream.h>

#include "xmem/TestMem.h"
#include "xmem/MemMsg.h"

namespace xmem {

  #ifdef XILINX_VIVADO_HLS_TESTING
  typedef xmem::TestMem  MemReqStream;
  typedef xmem::TestMem  MemRespStream;
  #else
  typedef hls::stream<MemReqMsg<> >  MemReqStream;
  typedef hls::stream<MemRespMsg<> > MemRespStream;
  #endif

  //XXX:This is only true for HLS or HLS_TESTING.
  //    For C++ compilation, PTR_SIZE should be sizeof(void*)
  //    which is 8 on brg
  const static unsigned PTR_SIZE = 4;

  typedef unsigned   Address;
  typedef ap_uint<8> Opaque;

}

#endif /* XMEM_MEM_COMMON_H */

