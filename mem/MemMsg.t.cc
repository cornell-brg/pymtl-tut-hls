//========================================================================
// Unit Tests for MemMsg
//========================================================================

#include "utst.h"
#include "MemMsg.h"

using namespace mem;

//------------------------------------------------------------------------
// Test Write Messages
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestWrite )
{

  //                              type   opq    addr len    data
  MemReqMsg req( MemReqMsg::TYPE_WRITE, 0x84, 0x1000,  2, 0xdeadbeef );

  UTST_CHECK_EQ( req.type(),          1 );
  UTST_CHECK_EQ( req.opq(),        0x84 );
  UTST_CHECK_EQ( req.addr(),     0x1000 );
  UTST_CHECK_EQ( req.len(),           2 );
  UTST_CHECK_EQ( req.data(), 0xdeadbeef );

  //                                type   opq len data
  MemRespMsg resp( MemReqMsg::TYPE_WRITE, 0x84,  2,   0 );

  UTST_CHECK_EQ( resp.type(),    1 );
  UTST_CHECK_EQ( resp.opq(),  0x84 );
  UTST_CHECK_EQ( resp.len(),     2 );
  UTST_CHECK_EQ( resp.data(),    0 );

}

//------------------------------------------------------------------------
// Test Read Messages
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestRead )
{

  //                             type   opq    addr len  data
  MemReqMsg req( MemReqMsg::TYPE_READ, 0x84, 0x1000,  4,    0 );

  UTST_CHECK_EQ( req.type(),      0 );
  UTST_CHECK_EQ( req.opq(),    0x84 );
  UTST_CHECK_EQ( req.addr(), 0x1000 );
  UTST_CHECK_EQ( req.len(),       0 );
  UTST_CHECK_EQ( req.data(),      0 );

  //                               type   opq len        data
  MemRespMsg resp( MemReqMsg::TYPE_READ, 0x84,  0, 0xdeadbeef );

  UTST_CHECK_EQ( resp.type(),          0 );
  UTST_CHECK_EQ( resp.opq(),        0x84 );
  UTST_CHECK_EQ( resp.len(),           0 );
  UTST_CHECK_EQ( resp.data(), 0xdeadbeef );

}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

