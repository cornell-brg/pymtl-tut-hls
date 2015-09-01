//========================================================================
// TestMem
//========================================================================
// This is a test memory class suitable for composition with an HLS
// accelerator that uses hls::streams to explicitly send/receive memory
// requests/responses.

#include "xmem/MemMsg.h"

#include <deque>
#include <iostream>

#ifndef XMEM_TEST_MEM_H
#define XMEM_TEST_MEM_H

namespace xmem {

  class TestMem
  {
   public:

    //--------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------

    TestMem();

    //--------------------------------------------------------------------
    // read/write
    //--------------------------------------------------------------------
    // Methods for interacting with the test memory as if it was two
    // hls::streams. This will let an accelerator write memory request
    // messages to the test memory, and then read the corresponding memory
    // responses.

    MemRespMsg<> read();
    void write( const MemReqMsg<>& memreq );

    //--------------------------------------------------------------------
    // mem_read/mem_write
    //--------------------------------------------------------------------
    // Simpler methods useful for allowing the test harness to initialize
    // and then verify data in memory. These do not count towards the
    // number of requests mentioned below.

    unsigned int mem_read( int addr );
    void mem_write( int addr, unsigned int data );

    //--------------------------------------------------------------------
    // num_requests
    //--------------------------------------------------------------------
    // Track number of requests that have been sent to the test memory.
    // This is useful for verying that only a specific number of memory
    // requests are generated for a given unit test.

    void clear_num_requests();
    int get_num_requests();
    int get_num_stores();
    int get_num_loads();

   private:

    void _write( const MemReqMsg<>& memreq );

    std::deque<MemRespMsg<> > m_memresp_q;
    ap_uint<8> m_mem[1<<20];
    int m_num_stores;
    int m_num_loads;

  };

}

#endif /* XMEM_TEST_MEM_H */

