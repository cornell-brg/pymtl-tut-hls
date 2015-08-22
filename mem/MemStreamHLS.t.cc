//========================================================================
// Unit Tests for MemStreamHLS
//========================================================================

#include "utst/utst.h"
#include "mem/MemStreamHLS.h"
#include "xcel/XcelMsg.h"

#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace xcel;
using namespace mem;

//------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------

void run_test( int test_num, int num_mem_reqs,
  unsigned int data_in_addr,  unsigned int data_in[],  int data_in_size,
  unsigned int data_out_addr, unsigned int data_out[], int data_out_size )
{
  // Test memory

  TestMem MemStreamHLS_mem;

  // Reset test memory request counter

  MemStreamHLS_mem.clear_num_requests();

  // Create configuration req/resp streams

  hls::stream<XcelReqMsg>  xcelreq;
  hls::stream<XcelRespMsg> xcelresp;

  // Clear results

  for ( int i = 0; i < data_out_size; i++ )
    MemStreamHLS_mem.mem_write( data_out_addr+(4*i), 0 );

  // Initialize array

  for ( int i = 0; i < data_in_size; i++ )
    MemStreamHLS_mem.mem_write( data_in_addr+(4*i), data_in[i] );

  xcelreq.write( XcelReqMsg( 0, XcelReqMsg::TYPE_WRITE, 1, test_num, 0 ) );

  // Do test

  MemStreamHLS( xcelreq, xcelresp, MemStreamHLS_mem, MemStreamHLS_mem  );

  // Drain the responses for configuration requests

  xcelresp.read();

  // Verify the results

  for ( int i = 0; i < data_out_size; i++ )
    UTST_CHECK_EQ( MemStreamHLS_mem.mem_read( data_out_addr+(4*i) ),
                   data_out[i] );

  UTST_CHECK_EQ( MemStreamHLS_mem.get_num_requests(), num_mem_reqs );
}

//------------------------------------------------------------------------
// Test Basic
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasic )
{
  unsigned int data_in[]  = { 0x0eadbeef                         };
  unsigned int data_out[] = { 0x0eadbeef, 0x0eadbeef, 0x0a0a0a0a };
  run_test( 0, 3, 0x1000, data_in, 1, 0x1000, data_out, 3 );
}

//------------------------------------------------------------------------
// Test Multiple
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMultiple )
{
  unsigned int data_in[]  = { 42,   13, 1024 };
  unsigned int data_out[] = { 1024, 13, 42,  0x0b0b0b0b, 0x0c0c0c0c, 0x0d0d0d0d };
  run_test( 1, 9, 0x1000, data_in, 3, 0x2000, data_out, 6 );
}

//------------------------------------------------------------------------
// Test Struct
//------------------------------------------------------------------------
// See MemStreamHLS.cc: Foo has three fields:
//
//  struct Foo {
//    int   a;
//    short b;
//    char  c;
//  };
//
// When packed into memory a Foo object will require 8B:
//
//  63            32 31            0
//  +---+---+---+---+---+---+---+---+
//  |pad| c | b | b | a | a | a | a |
//  +---+---+---+---+---+---+---+---+
//
// Or as an array of unsigned ints:
//
//  { 0xaaaaaaaa, 0x00ccbbbb }
//

UTST_AUTO_TEST_CASE( TestStruct )
{
  unsigned int data_in[]  = { 0x01234567, 0x00320582 };
  unsigned int data_out[] = { 0x01234567, 0x00320582, 0x00000042, 0x00780400 };
  run_test( 2, 9, 0x1000, data_in, 2, 0x2000, data_out, 4 );
}

//------------------------------------------------------------------------
// Test Mixed
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMixed )
{
  unsigned int data_in[]  = { 0x01234567, 0x00320582, 0x0a0a0a0a, 0x07654310, 0x00130450 };
  unsigned int data_out[] = { 0x0a0a0a0a, 0x01234567, 0x00130450, 0x07654310, 0x00320582 };
  run_test( 3, 14, 0x1000, data_in, 5, 0x2000, data_out, 5 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv, "mem" );
}

