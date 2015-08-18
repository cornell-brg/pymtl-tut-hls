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

  //                                   type   opq    addr len        data
  MemReqMsg<> req0( MemReqMsg<>::TYPE_WRITE, 0x84, 0x1000,  2, 0xdeadbeef );

  UTST_CHECK_EQ( req0.type(),          1 );
  UTST_CHECK_EQ( req0.opq(),        0x84 );
  UTST_CHECK_EQ( req0.addr(),     0x1000 );
  UTST_CHECK_EQ( req0.len(),           2 );
  UTST_CHECK_EQ( req0.data(), 0xdeadbeef );

  //                                      type   opq len data
  MemRespMsg<> resp0( MemRespMsg<>::TYPE_WRITE, 0x84,  2,   0 );

  UTST_CHECK_EQ( resp0.type(),    1 );
  UTST_CHECK_EQ( resp0.opq(),  0x84 );
  UTST_CHECK_EQ( resp0.len(),     2 );
  UTST_CHECK_EQ( resp0.data(),    0 );

  //                                        type   opq    addr len        data
  MemReqMsg<128,4> req1( MemReqMsg<>::TYPE_WRITE, 0x84, 0x1000, 16, 0xdeadbeef );

  UTST_CHECK_EQ( req1.type(),          1 );
  UTST_CHECK_EQ( req1.opq(),        0x84 );
  UTST_CHECK_EQ( req1.addr(),     0x1000 );
  UTST_CHECK_EQ( req1.len(),           0 );
  UTST_CHECK_EQ( req1.data(), 0xdeadbeef );

  //                                           type   opq len data
  MemRespMsg<128,4> resp1( MemRespMsg<>::TYPE_WRITE, 0x84,  5,   0 );

  UTST_CHECK_EQ( resp1.type(),    1 );
  UTST_CHECK_EQ( resp1.opq(),  0x84 );
  UTST_CHECK_EQ( resp1.len(),     5 );
  UTST_CHECK_EQ( resp1.data(),    0 );

}

//------------------------------------------------------------------------
// Test Read Messages
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestRead )
{

  //                                  type   opq    addr len  data
  MemReqMsg<> req0( MemReqMsg<>::TYPE_READ, 0x84, 0x1000,  4,    0 );

  UTST_CHECK_EQ( req0.type(),      0 );
  UTST_CHECK_EQ( req0.opq(),    0x84 );
  UTST_CHECK_EQ( req0.addr(), 0x1000 );
  UTST_CHECK_EQ( req0.len(),       0 );
  UTST_CHECK_EQ( req0.data(),      0 );

  //                                     type   opq len        data
  MemRespMsg<> resp0( MemRespMsg<>::TYPE_READ, 0x84,  0, 0xdeadbeef );

  UTST_CHECK_EQ( resp0.type(),          0 );
  UTST_CHECK_EQ( resp0.opq(),        0x84 );
  UTST_CHECK_EQ( resp0.len(),           0 );
  UTST_CHECK_EQ( resp0.data(), 0xdeadbeef );

  //                                       type   opq    addr len data
  MemReqMsg<128,4> req1( MemReqMsg<>::TYPE_READ, 0x84, 0x1000,  7,   0 );

  UTST_CHECK_EQ( req1.type(),      0 );
  UTST_CHECK_EQ( req1.opq(),    0x84 );
  UTST_CHECK_EQ( req1.addr(), 0x1000 );
  UTST_CHECK_EQ( req1.len(),       7 );
  UTST_CHECK_EQ( req1.data(),      0 );

  //                                          type   opq len            data
  MemRespMsg<128,4> resp1( MemRespMsg<>::TYPE_READ, 0x84,  8, 0x0a0a0b0b0000 );

  UTST_CHECK_EQ( resp1.type(),              0 );
  UTST_CHECK_EQ( resp1.opq(),            0x84 );
  UTST_CHECK_EQ( resp1.len(),               8 );
  UTST_CHECK_EQ( resp1.data(), 0x0a0a0b0b0000 );

}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

