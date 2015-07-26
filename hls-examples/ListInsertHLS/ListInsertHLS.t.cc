#include <ap_utils.h>

#include "../../itu_ptrs/common.h"
#include "../../itu_ptrs/interfaces.h"

#include "ListHLS.h"
#include <assert.h>

#define UTST_CHECK_EQ(x,y) assert((x)==(y))

typedef int Type;
typedef list<Type> List;

#ifndef CPP_COMPILE
hls::stream<MemReqMsg>  memreq;
hls::stream<MemRespMsg> memresq;
#endif

// hls top function
Address ListInsertHLS( Address pos, Address new_mem, const Type& val ) {
  NodeProxyPointer<Type> pos_node( pos );
  NodeProxyPointer<Type> new_node( new_mem );

  (*new_node).m_value = val;
  (*new_node).m_next = pos_node;
  (*new_node).m_prev = (*pos_node).m_prev;

  (*(*pos_node).m_prev).m_next = new_node;
  (*pos_node).m_prev = new_node;
  return new_node.get_addr();
}

List::iterator insert_hls( List::const_iterator pos, const Type& val ) {
  // allocate memory
  Address mem = (Address)malloc(sizeof(Type)+2*sizeof(Address));

  Address ret = ListInsertHLS( pos.get_ptr().get_addr(), mem, val );
  return List::iterator( NodeProxyPointer<Type>( ret ) );
}

//------------------------------------------------------------------------
// Test insert
//------------------------------------------------------------------------
void test_insert()
{
  List L1;
  List::iterator it;
  L1.push_back(2);
  L1.push_back(4);

  printf ("TEST: inserting 1\n");
  // insert 1 at beginning of list
  it = L1.begin();
  insert_hls(it, 1);

  printf ("TEST: inserting 5\n");
  // insert 5 at end
  insert_hls(L1.end(), 5);
  
  printf ("TEST: inserting 3\n");
  // insert 3 between 2 and 4
  it = L1.begin();
  while ( (int)(*it) != 4 ) {
    ++it;
  }
  insert_hls(it, 3);

  UTST_CHECK_EQ( L1.size(), 5 );

  int i = 1;
  printf ("TEST: checking list\n");
  for (it = L1.begin(); it != L1.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }

  L1.clear();
  UTST_CHECK_EQ( L1.size(), 0 );
}

int main () {
  test_insert();
  printf("#### list insert tests completed ####\n");
  return 0;
}



