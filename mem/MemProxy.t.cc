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

  MemValue<int> a(0x1000);
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

  MemValue<int> a(0x2000);
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

  MemValue<int> a(0x1000);
  MemValue<int> b(0x2000);

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

  MemValue<int> a(0x1000);
  MemValue<int> b(0x2000);

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

  MemValue<int> a(0x1000);
  int value0 = a; // mem read
  int value1 = a; // no mem read!

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 1 );
  UTST_CHECK_EQ( value0, 0x0afecafe );
  UTST_CHECK_EQ( value1, 0x0afecafe );

  g_test_mem.clear_num_requests();

  MemValue<int> b(0x2000);
  b = 42;        // mem write
  int valueb0 = b; // mem read
  int valueb1 = b; // mem read

  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 2 );
  UTST_CHECK_EQ( valueb0, 42 );
  UTST_CHECK_EQ( valueb1, 42 );
}

//------------------------------------------------------------------------
// Test Pointer
//------------------------------------------------------------------------
// Ack -- this is why memoization doesn't work. The second read of a in
// the UTST_CHECK_EQ gets the memoized copy not the value updated through
// the pointer.
// RZ: We can modify MemPointer to remember a MemValue, then have
// the & operator of MemValue return a MemPointer which remembers it,
// which solves this issue.

UTST_AUTO_TEST_CASE( TestPointer )
{
  g_test_mem.clear_num_requests();

  // Get pointer from referencing a value
  MemValue<int>   a(0x1000);
  MemPointer<int> ap = &a;

  a = 42;
  UTST_CHECK_EQ( a, 42 );   // load: a is memoized
  *ap = 13;
  UTST_CHECK_EQ( a, 13 );  

  MemPointer<int> a_ptr2 = ap;
  a = 45;
  UTST_CHECK_EQ( a, 45 );   // load: a is memoized
  *a_ptr2 = 23;
  UTST_CHECK_EQ( a, 23 );  
}

// RZ: However, in this case the * operator of MemPointer
// must return a MemValue, and it doesn't know about pre-existing
// MemValues.
/*UTST_AUTO_TEST_CASE( TestPointer2 )
{
  g_test_mem.clear_num_requests();
  
  // Get value from dereferencing a pointer
  MemPointer<int> b_ptr(0x2000);
  MemValue<int>   b = *b_ptr;

  b = 47;
  UTST_CHECK_EQ( b, 47 );   // load: b is memoized
  *b_ptr = 14;
  UTST_CHECK_EQ( b, 14 );
}*/

//------------------------------------------------------------------------
// Test Pointer Assignment
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestPointerAssign )
{
  g_test_mem.clear_num_requests();

  MemValue<int> a(0x1000);
  MemValue<int> b(0x2000);
  a = 20;   // write 1
  b = 30;   // write 2

  MemPointer<int> ap = &b;
  MemPointer<int> bp = &a;
  ap = bp;
  bp = &b;

  // only 2 writes
  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 2 );

  UTST_CHECK_EQ( a, 20 );
  UTST_CHECK_EQ( b, 30 );
  UTST_CHECK_EQ( *ap, 20 );
  UTST_CHECK_EQ( *bp, 30 );
}

//------------------------------------------------------------------------
// Test Pointer in memory
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestPointerInMem )
{
  g_test_mem.clear_num_requests();

  MemValue<int> a(0x1000);
  MemValue<int> b(0x1004);

  a = 42;
  b = 47;

  MemValue< MemPointer<int> > a_ptr(0x1008);
  MemValue< MemPointer<int> > b_ptr(0x100c);

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

struct Node
{
  Node* m_prev;
  Node* m_next;
  int m_data;
};

namespace mem {

  template <>
  class MemValue< Node > {
    public:
      MemValue< MemPointer< Node > > m_prev;
      MemValue< MemPointer< Node > > m_next;
      MemValue< int > m_data;

      // Constructor
      MemValue( Address base_ptr )
      : m_prev( base_ptr ),
        m_next( base_ptr + PTR_SIZE),
        m_data( base_ptr + PTR_SIZE + PTR_SIZE)
      {}

      // Get Address
      void set_addr( const Address addr ) {
        m_prev.set_addr( addr );
        m_next.set_addr( addr+PTR_SIZE );
        m_data.set_addr( addr+PTR_SIZE+PTR_SIZE );
      }

      // Set Address
      Address get_addr() const {
        return m_prev.get_addr();
      }

      static size_t size() {
        return 2*PTR_SIZE + MemValue<int>::size();
      }

      void unmemoize() {}
  };

};  // end namespace mem

UTST_AUTO_TEST_CASE( TestArrow )
{
  g_test_mem.clear_num_requests();

  MemPointer<Node> p1( 0x1000 );
  MemPointer<Node> p2( 0x2000 );
  MemValue<Node> n1 = *p1;
  MemValue<Node> n2 = *p2;

  p1->m_prev = p1;    // 1 req
  p1->m_next = p2;    // 1 req
  p1->m_data = 12;    // 1 req

  p1->m_next->m_prev = p1;  // 2 reqs
  p1->m_next->m_next = p2;  // 1 reqs
  p1->m_next->m_data = 23;  // 1 reqs
  
  //UTST_CHECK_EQ( g_test_mem.get_num_requests(), 7 );
  UTST_CHECK_EQ( 12, n1.m_data );
  UTST_CHECK_EQ( 23, n2.m_data );

  g_test_mem.clear_num_requests();

  n1.m_data = 45;
  n2.m_data = 67;
  UTST_CHECK_EQ( g_test_mem.get_num_requests(), 2 );
  UTST_CHECK_EQ( 45, p1->m_data );
  UTST_CHECK_EQ( 67, p2->m_data );
}

// Currently we don't support implicit bool conversion 
// So you have to compare against a number
UTST_AUTO_TEST_CASE( TestLogical )
{
  MemPointer<int> p0( 0x0000 );
  MemPointer<int> p1( 0x1000 );

  //if ( p0 )  { UTST_CHECK_EQ( 0, 1 ); }
  if ( p0 != 0 )  { UTST_CHECK_EQ( 0, 1 ); }
  if ( !p1 ) { UTST_CHECK_EQ( 0, 1 ); }
  if ( p1 == 0 ) { UTST_CHECK_EQ( 0, 1 ); }

  //if ( p0 && p1 ) { UTST_CHECK_EQ( 0, 1 ); }
  //if ( p0 && p1 != 0 ) { UTST_CHECK_EQ( 0, 1 ); }
  //if ( p0 == 0 && p1 ) { UTST_CHECK_EQ( 0, 1 ); }
  if ( p0 == 0 && p1 == 0) { UTST_CHECK_EQ( 0, 1 ); }
  
  //if ( p0 || !p1 ) { UTST_CHECK_EQ( 0, 1 ); }
  //if ( p0 || p1 == 0 ) { UTST_CHECK_EQ( 0, 1 ); }
  //if ( p0 == 0 || p1 ) { UTST_CHECK_EQ( 0, 1 ); }
  if ( p0 != 0 || p1 == 0 ) { UTST_CHECK_EQ( 0, 1 ); }
}

UTST_AUTO_TEST_CASE( TestArrowLogical )
{
  MemPointer<Node> p( 0x1000 );
  p->m_prev = 0x0;
  p->m_next = 0x8;

  if ( p->m_prev != 0 )  { UTST_CHECK_EQ( 0, 1 ); }
  if ( !p->m_next ) { UTST_CHECK_EQ( 0, 1 ); }
  if ( p->m_next == 0 ) { UTST_CHECK_EQ( 0, 1 ); }

  if ( p->m_prev != 0 && p->m_next != 0 ) {
    UTST_CHECK_EQ( 0, 1 );
  }
  if ( p != 0 && p->m_next == 0 ) {
    UTST_CHECK_EQ( 0, 1 );
  }
  if ( p->m_prev != 0 && p == 0 ) {
    UTST_CHECK_EQ( 0, 1 );
  }
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::auto_command_line_driver( argc, argv );
}

