//========================================================================
// TestMem
//========================================================================

#include "TestMem.h"
#include "utst.h"

namespace mem {

  //----------------------------------------------------------------------
  // TestMem::TestMem
  //----------------------------------------------------------------------

  TestMem::TestMem()
   : m_num_requests(0)
  {}

  //----------------------------------------------------------------------
  // TestMem::write
  //----------------------------------------------------------------------
  // We use an internal write method so that we can only display the utst
  // debug information when we are actualy writing the test memory from
  // inside an accelerator (as opposed to using the mem_write method).

  void TestMem::_write( const MemReqMsg<>& memreq )
  {
    // Handle read requests

    if ( memreq.type() == MemReqMsg<>::TYPE_READ ) {

      // Increment our message request counter

      m_num_requests++;

      // Set length to four bytes if len field is zero

      int len = ( memreq.len() == 0 ) ? 4 : static_cast<int>(memreq.len());

      // Iterate over bytes to create data for response

      ap_uint<32> data = 0;
      if ( len == 4 ) {
        data = ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 3 ]) << 24 )
             | ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 2 ]) << 16 )
             | ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 1 ]) << 8  )
             | ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 0 ])       );
      }
      else if ( len == 2 ) {
        data = ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 1 ]) << 8  )
             | ( static_cast< ap_uint<32> >(m_mem[ static_cast<int>(memreq.addr()) + 0 ])       );
      }
      else if ( len == 1 ) {
        data = m_mem[ static_cast<int>(memreq.addr()) ];
      }

      // Create the response message and enqueue

      MemRespMsg<> resp( 0, memreq.opq(), memreq.len(), data );

      m_memresp_q.push_back( resp );
    }

    // Handle write requests

    else if ( memreq.type() == MemReqMsg<>::TYPE_WRITE ) {

      // Increment our message request counter

      m_num_requests++;

      // Set length to four bytes if len field is zero

      int len = ( memreq.len() == 0 ) ? 4 : static_cast<int>(memreq.len());

      // Iterate over bytes to write data

      ap_uint<32> data = memreq.data();
      for ( int i = 0; i < len; i++ ) {
        m_mem[ static_cast<int>(memreq.addr()) + i ] = static_cast< ap_uint<8> >(data);
        data = data >> 8;
      }

      // Create the response message and enqueue

      MemRespMsg<> resp( 1, memreq.opq(), memreq.len(), 0 );

      m_memresp_q.push_back( resp );
    }

  }

  //----------------------------------------------------------------------
  // TestMem::write
  //----------------------------------------------------------------------

  void TestMem::write( const MemReqMsg<>& memreq )
  {
    using namespace std;

    if ( memreq.type() == MemReqMsg<>::TYPE_READ ) {

      UTST_LOG_MSG( "TestMem: rd:"
        << setfill('0') << setw(2) << hex << static_cast<unsigned int>(memreq.opq())  << ":"
        << setfill('0') << setw(8) << hex << static_cast<unsigned int>(memreq.addr()) << ":"
      );

    } else if ( memreq.type() == MemReqMsg<>::TYPE_WRITE ) {

      UTST_LOG_MSG( "TestMem: wr:"
        << setfill('0') << setw(2) << hex << static_cast<unsigned int>(memreq.opq())  << ":"
        << setfill('0') << setw(8) << hex << static_cast<unsigned int>(memreq.addr()) << ":"
        << setfill('0') << setw(8) << hex << static_cast<unsigned int>(memreq.data())
      );

    }

    _write( memreq );
  }

  //----------------------------------------------------------------------
  // TestMem::read
  //----------------------------------------------------------------------

  MemRespMsg<> TestMem::read()
  {
    MemRespMsg<> resp = m_memresp_q.front();
    m_memresp_q.pop_front();
    return resp;
  }

  //----------------------------------------------------------------------
  // TestMem::mem_write
  //----------------------------------------------------------------------

  void TestMem::mem_write( int addr, unsigned int data )
  {
    _write( MemReqMsg<>( 1, 0, addr, 0, data ) );

    // We don't want to count this as a memory request since it is part of
    // the test harness.

    m_num_requests--;

    MemRespMsg<> resp = read();
  }

  //----------------------------------------------------------------------
  // TestMem::mem_read
  //----------------------------------------------------------------------

  unsigned int TestMem::mem_read( int addr )
  {
    _write( MemReqMsg<>( 0, 0, addr, 0, 0 ) );

    // We don't want to count this as a memory request since it is part of
    // the test harness.

    m_num_requests--;

    MemRespMsg<> resp = read();
    return resp.data();
  }

  //----------------------------------------------------------------------
  // TestMem::clear_num_requests
  //----------------------------------------------------------------------

  void TestMem::clear_num_requests()
  {
    m_num_requests = 0;
  }

  //----------------------------------------------------------------------
  // TestMem::get_num_requests
  //----------------------------------------------------------------------

  int TestMem::get_num_requests()
  {
    return m_num_requests;
  }

}
