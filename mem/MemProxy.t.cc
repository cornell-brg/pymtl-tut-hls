//========================================================================
// Unit Tests for MemProxy
//========================================================================

#include "utst.h"
#include "TestMem.h"
#include "MemStream.h"
#include "MemProxy.h"

using namespace mem;

TestMem g_test_mem;
TestMem& memreq  = g_test_mem;
TestMem& memresp = g_test_mem;

//------------------------------------------------------------------------
// Test Basic Write
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasicWrite )
{
  g_test_mem.clear_num_requests();

  MemProxy<int> a(0x1000);
  a = 42; // mem write

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x1000), 42u );
}

//------------------------------------------------------------------------
// Test Basic Read
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestBasicRead )
{
  g_test_mem.clear_num_requests();

  g_test_mem.mem_write( 0x2000, 0xdeadbeef );

  MemProxy<int> a(0x2000);
  unsigned int value = a; // mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( value, 0xdeadbeefu );
}

//------------------------------------------------------------------------
// Test Copy
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestCopy )
{
  g_test_mem.clear_num_requests();

  g_test_mem.mem_write( 0x1000, 13u );

  MemProxy<int> a(0x1000);
  MemProxy<int> b(0x2000);

  b = a;

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 2 );
  UTST_CHECK_EQ( g_test_mem.mem_read(0x2000), 13u );
}

//------------------------------------------------------------------------
// Test Operators
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestOperators )
{
  g_test_mem.clear_num_requests();

  MemProxy<int> a(0x1000);
  MemProxy<int> b(0x2000);

  a = 42;
  b = 47;

  bool eq  = ( a == b );
  bool neq = ( a != b );
  bool lt  = ( a <  b );
  bool gt  = ( a >  b );

  UTST_CHECK_EQ( eq,  false );
  UTST_CHECK_EQ( neq, true  );
  UTST_CHECK_EQ( lt,  true  );
  UTST_CHECK_EQ( gt,  false );
}

//------------------------------------------------------------------------
// Test Memoize
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestMemoize )
{
  g_test_mem.clear_num_requests();

  g_test_mem.mem_write( 0x1000, 0x0afecafe );

  MemProxy<int> a(0x1000);
  int value0 = a; // mem read
  int value1 = a; // no mem read!

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( value0, 0x0afecafe );
  UTST_CHECK_EQ( value1, 0x0afecafe );

  g_test_mem.clear_num_requests();

  MemProxy<int> b(0x2000);
  b = 42;        // mem write
  int value = b; // no mem read!

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( value, 42 );
}

//------------------------------------------------------------------------
// Test Pointer
//------------------------------------------------------------------------
// Ack -- this is why memoization doesn't work. The second read of a in
// the UTST_CHECK_EQ gets the memoized copy not the value updated through
// the pointer.

UTST_AUTO_TEST_CASE( TestPointer )
{
  g_test_mem.clear_num_requests();

  MemProxy<int> a(0x1000);
  MemProxy<int> b(0x2000);

  a = 42;
  b = 47;

  MemPointer<int> a_ptr = &a;
  MemPointer<int> b_ptr = &b;

  UTST_CHECK_EQ( a, 42 );
  UTST_CHECK_EQ( b, 47 );

  *a_ptr = 13;
  *b_ptr = 14;

  UTST_CHECK_EQ( a, 13 );
  UTST_CHECK_EQ( b, 14 );
}

//------------------------------------------------------------------------
// Test Pointer in memory
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestPointerInMem )
{
  g_test_mem.clear_num_requests();

  MemProxy<int> a(0x1000);
  MemProxy<int> b(0x1004);

  a = 42;
  b = 47;

  MemProxy< MemPointer<int> > a_ptr(0x1008);
  MemProxy< MemPointer<int> > b_ptr(0x100c);

  a_ptr = &a;
  b_ptr = &b;

  *a_ptr = *b_ptr;

  UTST_CHECK_EQ( a, 47 );
  UTST_CHECK_EQ( b, 47 );

  *a_ptr = 13;
  *b_ptr = 14;

  UTST_CHECK_EQ( a, 13 );
  UTST_CHECK_EQ( b, 14 );
}

//------------------------------------------------------------------------
// Test Struct Proxy
//------------------------------------------------------------------------

struct Node0
{
  int a;
  int b;
};

namespace mem {

  OutMemStream& operator<<( OutMemStream& os, const Node0& rhs )
  {
    os << rhs.a << rhs.b;
    return os;
  }

  InMemStream& operator>>( InMemStream& is, Node0& rhs )
  {
    is >> rhs.a >> rhs.b;
    return is;
  }

  template <>
  class MemProxy<Node0> {

   public:

    explicit MemProxy( unsigned int addr )
      : a(addr), b(addr+4), m_addr(addr)
    { }

    // cast

    operator Node0() const
    {
      Node0 node;
      mem::InMemStream is(m_addr);
      is >> node;
      return node;
    }

    // lvalue use of the proxy object with value on RHS

    MemProxy<Node0>& operator=( const Node0& node )
    {
      mem::OutMemStream os(m_addr);
      os << node;
      return *this;
    }

    // lvalue use of the proxy object with proxy on RHS

    MemProxy<Node0>& operator=( const MemProxy<Node0>& rhs )
    {
      mem::OutMemStream os(m_addr);
      os << static_cast<Node0>(rhs);
      return *this;
    }

    // Address of operator

    MemPointer<Node0> operator&()
    {
      return MemPointer<Node0>(m_addr);
    }

    const MemPointer<Node0> operator&() const
    {
      return MemPointer<Node0>(m_addr);
    }

    MemProxy<int> a;
    MemProxy<int> b;

   private:

    unsigned int  m_addr;

  };

}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

