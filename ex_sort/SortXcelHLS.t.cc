//========================================================================
// Unit Tests for SortXcelHLS
//========================================================================

#include "utst.h"
#include "SortXcelHLS.h"

#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace xcel;
using namespace mem;

extern hls::stream<XcelReqMsg>  xcelreq;
extern hls::stream<XcelRespMsg> xcelresp;

//------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------

void run_test( const std::vector<int>& data )
{
  // Create configuration req/resp streams

  hls::stream<XcelReqMsg>  xcelreq;
  hls::stream<XcelRespMsg> xcelresp;

  // Test memory
  TestMem SortXcelHLS_mem;

  // Initialize array

  int size = static_cast<int>(data.size());
  for ( int i = 0; i < size; i++ )
    SortXcelHLS_mem.mem_write( 0x1000+(4*i), data[i] );

  // Insert configuration requests to do a sort

  //                         opq type  addr data    id
  xcelreq.write( XcelReqMsg( 0,     1,   1, 0x1000,  0 ) );
  xcelreq.write( XcelReqMsg( 0,     1,   2,   size,  0 ) );
  xcelreq.write( XcelReqMsg( 0,     0,   0,      0,  0 ) );

  // Do the sort

  SortXcelHLS(
              xcelreq,
              xcelresp,
              SortXcelHLS_mem,  // memreq
              SortXcelHLS_mem   // memresp
             );

  // Drain the responses for configuration requests

  xcelresp.read();
  xcelresp.read();
  xcelresp.read();

  // Create sorted vector for reference

  std::vector<int> data_ref = data;
  std::sort( data_ref.begin(), data_ref.end() );

  // Verify the results

  for ( int i = 0; i < size; i++ )
    UTST_CHECK_EQ( SortXcelHLS_mem.mem_read( 0x1000+(4*i) ), data_ref[i] );
}

//------------------------------------------------------------------------
// Test Mini
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMini )
{
  std::vector<int> data;
  data.push_back( 0x21 );
  data.push_back( 0x14 );
  data.push_back( 0x42 );
  data.push_back( 0x03 );

  run_test( data );
}

//------------------------------------------------------------------------
// Test Random
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestRandom )
{
  std::vector<int> data;
  for ( int i = 0; i < 32; i++ )
    data.push_back( rand() );

  run_test( data );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

