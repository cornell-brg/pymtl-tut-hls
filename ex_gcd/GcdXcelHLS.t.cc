//========================================================================
// Unit Tests for GcdXcelHLS
//========================================================================

#include "utst.h"
#include "GcdXcelHLS.h"

#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace xcel;

extern hls::stream<XcelReqMsg>  xcelreq;
extern hls::stream<XcelRespMsg> xcelresp;

//------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------

void run_test( const std::vector<std::pair<int,int> >& data,
               const std::vector<int>&                 ref   )
{

  // Create configuration req/resp streams

  hls::stream<XcelReqMsg>  xcelreq;
  hls::stream<XcelRespMsg> xcelresp;

  for ( unsigned i = 0; i < data.size(); ++ i )  {
    // Insert configuration requests to do compute gcd
    //                         opq type  addr data           id
    xcelreq.write( XcelReqMsg( 0,     1,   1, data[i].first,  0 ) );
    xcelreq.write( XcelReqMsg( 0,     1,   2, data[i].second, 0 ) );
    xcelreq.write( XcelReqMsg( 0,     0,   0, 0,              0 ) );

    // compute
    GcdXcelHLS( xcelreq, xcelresp );

    // Drain the response for writes
    xcelresp.read();
    xcelresp.read();

    // Verify the results
    XcelRespMsg resp = xcelresp.read();
    UTST_CHECK_EQ( resp.data(), ref[i] );

  }
}

//------------------------------------------------------------------------
// Test Basic
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasic )
{
  std::vector<std::pair<int,int> > data;
  std::vector<int>                 ref;

  data.push_back( std::make_pair( 15,  5 ) ); ref.push_back(  5 );
  data.push_back( std::make_pair(  9,  3 ) ); ref.push_back(  3 );
  data.push_back( std::make_pair(  0,  0 ) ); ref.push_back(  0 );
  data.push_back( std::make_pair( 27, 15 ) ); ref.push_back(  3 );
  data.push_back( std::make_pair( 21, 49 ) ); ref.push_back(  7 );
  data.push_back( std::make_pair( 25, 30 ) ); ref.push_back(  5 );
  data.push_back( std::make_pair( 19, 27 ) ); ref.push_back(  1 );
  data.push_back( std::make_pair( 40, 40 ) ); ref.push_back( 40 );

  run_test( data, ref );
}

//------------------------------------------------------------------------
// Test Random
//------------------------------------------------------------------------

int gcd( int opA, int opB ) {

  while ( opA != opB ) {
    if ( opA > opB )
      opA = opA - opB;
    else
      opB = opB - opA;
  }
  return opA;
}

UTST_AUTO_TEST_CASE( TestRandom )
{
  std::vector<std::pair<int,int> > data;
  std::vector<int>                 ref;

  for ( int i = 0; i < 32; i++ ) {
    int a = std::rand() % 100;
    int b = std::rand() % 100;
    data.push_back( std::make_pair(a,b) ); ref.push_back( gcd( a, b ) );
  }

  run_test( data, ref );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

