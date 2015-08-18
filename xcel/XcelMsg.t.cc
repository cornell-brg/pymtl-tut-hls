//========================================================================
// Unit Tests for XcelMsg
//========================================================================

#include "utst.h"
#include "XcelMsg.h"

using namespace mem;

//------------------------------------------------------------------------
// Test Write Messages
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestWrite )
{

  //               opq                    type  addr       data   id
  XcelReqMsg req( 0x53, XcelReqMsg::TYPE_WRITE,  31, 0xdeadbeef, 1024 );

  UTST_CHECK_EQ( req.opq(),        0x53 );
  UTST_CHECK_EQ( req.type(),          1 );
  UTST_CHECK_EQ( req.addr(),         31 );
  UTST_CHECK_EQ( req.data(), 0xdeadbeef );
  UTST_CHECK_EQ( req.id(),         1024 );

  //                 opq                    type  data   id
  XcelRespMsg resp( 0x53, XcelReqMsg::TYPE_WRITE,    0, 1024 );

  UTST_CHECK_EQ( resp.opq(),  0x53 );
  UTST_CHECK_EQ( resp.type(),    1 );
  UTST_CHECK_EQ( resp.data(),    0 );
  UTST_CHECK_EQ( resp.id(),   1024 );

}

//------------------------------------------------------------------------
// Test Read Messages
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestRead )
{

  //               opq                   type  addr data   id
  XcelReqMsg req( 0x84, XcelReqMsg::TYPE_READ,  31,    0,   1 );

  UTST_CHECK_EQ( req.opq(),  0x84 );
  UTST_CHECK_EQ( req.type(),    0 );
  UTST_CHECK_EQ( req.addr(),   31 );
  UTST_CHECK_EQ( req.data(),    0 );
  UTST_CHECK_EQ( req.len(),     1 );

  //                 opq                   type  data        id
  XcelRespMsg resp( 0x53, XcelReqMsg::TYPE_READ, 0xabcdeeff, 1 );

  UTST_CHECK_EQ( resp.opq(),        0x53 );
  UTST_CHECK_EQ( resp.type(),          0 );
  UTST_CHECK_EQ( resp.data(), 0xabcdeeff );
  UTST_CHECK_EQ( resp.id(),            1 );

}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

