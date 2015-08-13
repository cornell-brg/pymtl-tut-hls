//========================================================================
// Unit Tests for MemProxyHLS
//========================================================================

#include "utst.h"
#include "MemProxyHLS.h"
#include "XcelMsg.h"

#include <vector>

using namespace xcel;

//------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------

void run_test( int test_num, int num_mem_reqs,
  unsigned int data_in_addr,  unsigned int data_in[],  int data_in_size,
  unsigned int data_out_addr, unsigned int data_out[], int data_out_size )
{
  // Reset test memory request counter

  MemProxyHLS_mem.clear_num_requests();

  // Create configuration req/resp streams

  hls::stream<XcelReqMsg>  xcelreq;
  hls::stream<XcelRespMsg> xcelresp;

  // Clear results

  for ( int i = 0; i < data_out_size; i++ )
    MemProxyHLS_mem.mem_write( data_out_addr+(4*i), 0 );

  // Initialize array

  for ( int i = 0; i < data_in_size; i++ )
    MemProxyHLS_mem.mem_write( data_in_addr+(4*i), data_in[i] );

  xcelreq.write( XcelReqMsg( 0, test_num, 1, XcelReqMsg::TYPE_WRITE, 0 ) );

  // Do test

  MemProxyHLS( xcelreq, xcelresp );

  // Drain the responses for configuration requests

  xcelresp.read();

  // Verify the results

  for ( int i = 0; i < data_out_size; i++ )
    UTST_CHECK_EQ( MemProxyHLS_mem.mem_read( data_out_addr+(4*i) ),
                   data_out[i] );

  UTST_CHECK_EQ( MemProxyHLS_mem.get_num_requests(), num_mem_reqs );
}

//------------------------------------------------------------------------
// Test Basic Write
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasicWrite )
{
  unsigned int data_in[]  = {    };
  unsigned int data_out[] = { 42 };
  run_test( 0, 1, 0x1000, data_in, 0, 0x1000, data_out, 1 );
}

//------------------------------------------------------------------------
// Test Copy
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestCopy )
{
  unsigned int data_in[]  = { 13 };
  unsigned int data_out[] = { 13 };
  run_test( 1, 2, 0x1000, data_in, 1, 0x2000, data_out, 1 );
}

//------------------------------------------------------------------------
// Test Operators
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestOperators )
{
  unsigned int data_in[]  = { };
  unsigned int data_out[] = { 42, 47, 0x00010100 };
  run_test( 2, 6, 0x1000, data_in, 0, 0x2000, data_out, 3 );
}

//------------------------------------------------------------------------
// Test Memoize
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMemoize )
{
  unsigned int data_in[]  = { 0x0eadbeef };
  unsigned int data_out[] = { 0x0eadbeef, 0x0eadbeef, 0x0eadbeef };
  run_test( 3, 4, 0x1000, data_in, 1, 0x2000, data_out, 3 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

