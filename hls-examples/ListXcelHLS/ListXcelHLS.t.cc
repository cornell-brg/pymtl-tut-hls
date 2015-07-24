//========================================================================
// PolyHS list implementation unit tests
//========================================================================

#include "ListXcelHLS.h"
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

  // insert 1 at beginning of list
  list<int>::iterator it = L1.begin();
  L1.insert(it, 1);

  // insert 5 at end
  L1.insert(L1.end(), 5);
  
  // insert 3 between 2 and 4
  it = L1.begin();
  while ( *it != 4 ) ++it;
  L1.insert(it, 3);

  UTST_CHECK_EQ( L1.size(), 5 );

  int i = 1;
  for (it = L1.begin(); it != L1.end(); ++it) {
    printf ("%d %d\n", *it, i);
    UTST_CHECK_EQ( *it, i++ );
  }

  L1.clear();
  UTST_CHECK_EQ( L1.size(), 0 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  test_insert();
  printf ("list tests completed\n");
  return 0;
}
