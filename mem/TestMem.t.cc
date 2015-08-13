//========================================================================
// Unit Tests for TestMem
//========================================================================

#include "utst.h"
#include "TestMem.h"

//------------------------------------------------------------------------
// read helper function
//------------------------------------------------------------------------

void read( TestMem* mem, int addr, unsigned int data, int nbytes, int opaque )
{
  int len = ( nbytes == 4 ) ? 0 : nbytes;
  mem->write( MemReqMsg( 0, len, addr, opaque, 0 ) );
  MemRespMsg resp = mem->read();

  UTST_CHECK_EQ( resp.data, data   );
  UTST_CHECK_EQ( resp.len,  len    );
  UTST_CHECK_EQ( resp.opq,  opaque );
  UTST_CHECK_EQ( resp.type, 0      );
}

//------------------------------------------------------------------------
// write helper function
//------------------------------------------------------------------------

void write( TestMem* mem, int addr, unsigned int data, int nbytes, int opaque )
{
  int len = ( nbytes == 4 ) ? 0 : nbytes;
  mem->write( MemReqMsg( data, len, addr, opaque, 1 ) );
  MemRespMsg resp = mem->read();

  UTST_CHECK_EQ( resp.data, 0      );
  UTST_CHECK_EQ( resp.len,  len    );
  UTST_CHECK_EQ( resp.opq,  opaque );
  UTST_CHECK_EQ( resp.type, 1      );
}

//------------------------------------------------------------------------
// Test Basic
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasic )
{
  TestMem mem;
  write ( &mem, 0x1000, 99, 1, 42 );
  read  ( &mem, 0x1000, 99, 1, 13 );

  UTST_CHECK_EQ( mem.get_num_requests(), 2 );
}

//------------------------------------------------------------------------
// Test 1B
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( Test1B )
{
  TestMem mem;

  write ( &mem, 0x1000, 0xab, 1, 42 );
  read  ( &mem, 0x1000, 0xab, 1, 13 );

  write ( &mem, 0x1001, 0xcd, 1, 42 );
  read  ( &mem, 0x1001, 0xcd, 1, 13 );

  write ( &mem, 0x1002, 0xef, 1, 42 );
  read  ( &mem, 0x1002, 0xef, 1, 13 );

  write ( &mem, 0x1003, 0x01, 1, 42 );
  read  ( &mem, 0x1003, 0x01, 1, 13 );

  write ( &mem, 0x2000, 0xab, 1, 42 );
  write ( &mem, 0x2001, 0xcd, 1, 43 );
  write ( &mem, 0x2002, 0xef, 1, 44 );
  write ( &mem, 0x2003, 0x01, 1, 45 );

  read  ( &mem, 0x2000, 0xab, 1, 13 );
  read  ( &mem, 0x2001, 0xcd, 1, 14 );
  read  ( &mem, 0x2002, 0xef, 1, 15 );
  read  ( &mem, 0x2003, 0x01, 1, 16 );

  UTST_CHECK_EQ( mem.get_num_requests(), 16 );
}

//------------------------------------------------------------------------
// Test 2B
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( Test2B )
{
  TestMem mem;

  write ( &mem, 0x1000, 0xabcd, 2, 42 );
  read  ( &mem, 0x1000, 0xabcd, 2, 13 );

  write ( &mem, 0x1002, 0x0123, 2, 42 );
  read  ( &mem, 0x1002, 0x0123, 2, 13 );

  read  ( &mem, 0x1001, 0x23ab, 2, 13 );

  write ( &mem, 0x2000, 0xabcd, 2, 42 );
  write ( &mem, 0x2002, 0x0123, 2, 43 );

  read  ( &mem, 0x2000, 0xabcd, 2, 13 );
  read  ( &mem, 0x2002, 0x0123, 2, 14 );
  read  ( &mem, 0x2001, 0x23ab, 2, 15 );

  UTST_CHECK_EQ( mem.get_num_requests(), 10 );
}

//------------------------------------------------------------------------
// Test 4B
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( Test4B )
{
  TestMem mem;

  write ( &mem, 0x1000, 0xdeadbeef, 4, 42 );
  read  ( &mem, 0x1000, 0xdeadbeef, 4, 13 );

  write ( &mem, 0x2000, 0x0a0b0c0d, 4, 43 );
  write ( &mem, 0x2004, 0x01020304, 4, 44 );
  write ( &mem, 0x2008, 0x05060708, 4, 45 );
  write ( &mem, 0x200c, 0x090a0b0c, 4, 46 );

  read  ( &mem, 0x2000, 0x0a0b0c0d, 4, 47 );
  read  ( &mem, 0x2004, 0x01020304, 4, 48 );
  read  ( &mem, 0x2008, 0x05060708, 4, 49 );
  read  ( &mem, 0x200c, 0x090a0b0c, 4, 40 );

  UTST_CHECK_EQ( mem.get_num_requests(), 10 );
}

//------------------------------------------------------------------------
// Test mem read/write
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMemReadWrite )
{
  TestMem mem;

  mem.mem_write( 0x2000, 0x0a0b0c0d );
  mem.mem_write( 0x2004, 0x01020304 );
  mem.mem_write( 0x2008, 0x05060708 );
  mem.mem_write( 0x200c, 0x090a0b0c );

  UTST_CHECK_EQ( mem.mem_read( 0x2000 ), 0x0a0b0c0du );
  UTST_CHECK_EQ( mem.mem_read( 0x2004 ), 0x01020304u );
  UTST_CHECK_EQ( mem.mem_read( 0x2008 ), 0x05060708u );
  UTST_CHECK_EQ( mem.mem_read( 0x200c ), 0x090a0b0cu );

  UTST_CHECK_EQ( mem.get_num_requests(), 0 );
}

//------------------------------------------------------------------------
// Test num requests
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestNumRequests )
{
  TestMem mem;

  write ( &mem, 0x2000, 0x0a0b0c0d, 4, 13 );
  read  ( &mem, 0x2000, 0x0a0b0c0d, 4, 42 );

  write ( &mem, 0x2004, 0x01020304, 4, 14 );
  read  ( &mem, 0x2004, 0x01020304, 4, 43 );

  UTST_CHECK_EQ( mem.get_num_requests(), 4 );

  mem.clear_num_requests();

  write ( &mem, 0x2008, 0x05060708, 4, 15 );
  read  ( &mem, 0x2008, 0x05060708, 4, 44 );

  UTST_CHECK_EQ( mem.get_num_requests(), 2 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

