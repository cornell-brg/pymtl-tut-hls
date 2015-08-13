//========================================================================
// Unit Tests for MemStream
//========================================================================

#include "utst.h"
#include "MemStream.h"
#include "TestMem.h"

TestMem g_test_mem;
TestMem& memreq  = g_test_mem;
TestMem& memresp = g_test_mem;

//------------------------------------------------------------------------
// Test Basic Write
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasicWrite )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);
  int a = 42;
  os << a; // mem write

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1000), 42u );
}

//------------------------------------------------------------------------
// Test Basic Read
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasicRead )
{
  g_test_mem.clear_num_requests();

  g_test_mem.mem_write( 0x1000, 0xdeadbeef );

  mem::InMemStream is(0x1000);
  unsigned int a;
  is >> a; // mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( a, 0xdeadbeefu );
}

//------------------------------------------------------------------------
// Test Basic
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasic )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);
  int a = 42;
  os << a; // mem write

  mem::InMemStream is(0x1000);
  int b;
  is >> b; // mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 2 );
  UTST_CHECK_EQ( b, 42 );
}

//------------------------------------------------------------------------
// Test Multiple
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMultiple )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);
  int a = 42;
  int b = 13;
  int c = 1024;
  os << a << b << c; // 3x mem write

  mem::InMemStream is(0x1000);
  int d;
  int e;
  int f;
  is >> d >> e >> f; // 3x mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 6 );
  UTST_CHECK_EQ( d, 42 );
  UTST_CHECK_EQ( e, 13 );
  UTST_CHECK_EQ( f, 1024 );
}

//------------------------------------------------------------------------
// Example Struct
//------------------------------------------------------------------------

struct Foo {
  int   a;
  short b;
  char  c;
};

bool operator==( const Foo& lhs, const Foo& rhs )
{
  return (lhs.a == rhs.a) && (lhs.b == rhs.b) && (lhs.c == rhs.c);
}

std::ostream& operator<<( std::ostream& os, const Foo& foo )
{
  os << "{" << foo.a << "," << foo.b << "," << foo.c << "}";
  return os;
}

namespace mem {

  OutMemStream& operator<<( OutMemStream& os, const Foo& foo )
  {
    os << foo.a << foo.b << foo.c;
    os.addr += 1; // padding
    return os;
  }

  InMemStream& operator>>( InMemStream& is, Foo& foo )
  {
    is >> foo.a >> foo.b >> foo.c;
    is.addr += 1; // padding
    return is;
  }

}

//------------------------------------------------------------------------
// Test Struct
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestStruct )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);
  Foo foo1;
  foo1.a = 42;
  foo1.b = 1024;
  foo1.c = 'x';
  os << foo1; // 3x mem write

  // foo1 packed into memory will require 8B:
  //
  //  63            32 31            0
  //  +---+---+---+---+---+---+---+---+
  //  |pad| c | b | b | a | a | a | a |
  //  +---+---+---+---+---+---+---+---+
  //
  // Note that b in hex is 0x0400 and 'x' in hex is 0x78. So the 4B at
  // address 0x1000 should be a (i.e., 41) and the 4B at address 0x1004
  // should be b packed with c (i.e., 0x00780400).

  UTST_CHECK_EQ( g_test_mem.mem_read(0x1000), 42u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1004), 0x00780400u );

  mem::InMemStream is(0x1000);
  Foo foo2;
  is >> foo2; // 3x mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 6 );
  UTST_CHECK_EQ( foo1, foo2 );
}

//------------------------------------------------------------------------
// Test Struct Multiple
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestStructMultiple )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);

  Foo foo1;
  foo1.a = 42;
  foo1.b = 1024;
  foo1.c = 'x';
  os << foo1; // 3x mem write

  Foo foo2;
  foo2.a = 13;
  foo2.b = 2048;
  foo2.c = 'y';
  os << foo2; // 3x mem write

  // See comments in previous test case

  UTST_CHECK_EQ( g_test_mem.mem_read(0x1000), 42u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1004), 0x00780400u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1008), 13u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x100c), 0x00790800u );

  mem::InMemStream is(0x1000);
  Foo foo3, foo4;
  is >> foo3 >> foo4; // 6x mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 12 );
  UTST_CHECK_EQ( foo1, foo3 );
  UTST_CHECK_EQ( foo2, foo4 );
}

//------------------------------------------------------------------------
// Test Struct Mixed
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestStructMixed )
{
  g_test_mem.clear_num_requests();

  mem::OutMemStream os(0x1000);

  Foo foo1;
  foo1.a = 42;
  foo1.b = 1024;
  foo1.c = 'x';
  os << foo1;  // 3x mem write

  os << 67;    // 1x mem write

  Foo foo2;
  foo2.a = 13;
  foo2.b = 2048;
  foo2.c = 'y';
  os << foo2;  // 3x mem write

  // See comments in previous test case

  UTST_CHECK_EQ( g_test_mem.mem_read(0x1000), 42u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1004), 0x00780400u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1008), 67u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x100c), 13u );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1010), 0x00790800u );

  mem::InMemStream is(0x1000);
  Foo foo3, foo4;
  int  a;
  is >> foo3 >> a >> foo4; // 7x mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 14 );
  UTST_CHECK_EQ( foo1, foo3 );
  UTST_CHECK_EQ( a,    67   );
  UTST_CHECK_EQ( foo2, foo4 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

