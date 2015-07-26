//========================================================================
// PolyHS list implementation unit tests
//========================================================================

#include "ListProxy.h"
#include <stdio.h>
#include <assert.h>

#define UTST_CHECK_EQ(x,y) assert((x)==(y))

//------------------------------------------------------------------------
// Test insert
//------------------------------------------------------------------------
void test_insert()
{
  list<int> L1;
  L1.push_back(2);
  L1.push_back(4);

  printf ("TEST: inserting 1\n");
  // insert 1 at beginning of list
  list<int>::iterator it = L1.begin();
  L1.insert(it, 1);

  printf ("TEST: inserting 5\n");
  // insert 5 at end
  L1.insert(L1.end(), 5);
  
  printf ("TEST: inserting 3\n");
  // insert 3 between 2 and 4
  it = L1.begin();
  while ( (int)(*it) != 4 ) {
    ++it;
  }
  L1.insert(it, 3);

  UTST_CHECK_EQ( L1.size(), 5 );

  int i = 1;
  printf ("TEST: checking list\n");
  for (it = L1.begin(); it != L1.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }

  L1.clear();
  UTST_CHECK_EQ( L1.size(), 0 );
}

//------------------------------------------------------------------------
// Test erase
//------------------------------------------------------------------------
void test_erase()
{
  int i;
  list<int> L1;
  L1.push_back(1);
  L1.push_back(2);
  L1.push_back(3);
  L1.push_back(4);
  L1.push_back(5);

  printf ("TEST: erasing 1\n");
  list<int>::iterator it = L1.begin();
  L1.erase(it);
  UTST_CHECK_EQ( L1.size(), 4 );
  
  i = 2;
  for (it = L1.begin(); it != L1.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }

  printf ("TEST: erasing 5\n");
  it = L1.end();
  --it;
  L1.erase(it);
  UTST_CHECK_EQ( L1.size(), 3 );
  
  i = 2;
  for (it = L1.begin(); it != L1.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }
  
  printf ("TEST: erasing 3\n");
  it = L1.begin();
  while ( (int)(*it) != 3 ) {
    ++it;
  }
  L1.erase(it);
  UTST_CHECK_EQ( L1.size(), 2 );

  it = L1.begin();
  UTST_CHECK_EQ( *it, 2 );
  ++it;
  UTST_CHECK_EQ( *it, 4 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  test_insert();
  test_erase();
  printf ("#### list tests completed ####\n");
  return 0;
}
