//========================================================================
// PolyHS list implementation unit tests
//========================================================================

#include "List.h"
#include <stdio.h>
#include <assert.h>

#define UTST_CHECK_EQ(x,y) assert((x)==(y))

//------------------------------------------------------------------------
// Test Assignment Operator
//------------------------------------------------------------------------
void test_assign()
{
  const int N1 = 100;
  list<double> src;
  list<double>::iterator it;
  int i;

  for (int i = 0; i < N1; ++i) {
    src.push_back(i);
  }

  list<double> tmp;
  list<double> dst = tmp = src;
  src = src;
  
  UTST_CHECK_EQ( src.size(), N1 );
  UTST_CHECK_EQ( src.size(), tmp.size() );
  UTST_CHECK_EQ( src.size(), dst.size() );

  i = 0;
  for (it = src.begin(); it != src.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }

  i = 0;
  for (it = tmp.begin(); it != tmp.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }
  
  i = 0;
  for (it = dst.begin(); it != dst.end(); ++it) {
    UTST_CHECK_EQ( *it, i++ );
  }
}

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
// Test splice
//------------------------------------------------------------------------
void test_splice()
{
  list<int> L1, L2;
  L1.push_back(1);
  L1.push_back(2);
  L1.push_back(3);
  L2.push_back(4);
  L2.push_back(5);
  L2.push_back(6);

  list<int>::iterator it, old;
  int i;

  // test whole list splice
  old = L2.begin();
  L1.splice(L1.end(), L2);
  UTST_CHECK_EQ( L1.size(), 6 );
  UTST_CHECK_EQ( L2.size(), 0 );
  // check that iterator remains valid
  UTST_CHECK_EQ( *old, 4 );
  // check values for L1 = {1,2,3,4,5,6}
  i = 0;
  for (it = L1.begin(); it != L1.end(); ++it)
    UTST_CHECK_EQ( *it, ++i );

  // splice back to L2
  L2.splice(L2.begin(), L1, L1.begin(), old);
  UTST_CHECK_EQ( L1.size(), 3 );
  UTST_CHECK_EQ( L2.size(), 3 );
  UTST_CHECK_EQ( *old, 4 );
  // L1 = {4,5,6}
  // L2 = {1,2,3}
  i = 0;
  for (it = L2.begin(); it != L2.end(); ++it)
    UTST_CHECK_EQ( *it, ++i );
  for (it = L1.begin(); it != L1.end(); ++it)
    UTST_CHECK_EQ( *it, ++i );

  // test clear and resize too
  L1.clear();
  L2.resize(0);
  UTST_CHECK_EQ( L1.size(), 0 );
  UTST_CHECK_EQ( L2.size(), 0 );

  L1.push_back(1);
  L1.push_back(3);
  L1.push_back(4);
  L2.push_back(1);
  L2.push_back(2);
  L2.push_back(1);

  // test single element splice
  L1.splice(++L1.begin(), L2, ++L2.begin());
  UTST_CHECK_EQ( L1.size(), 4 );
  UTST_CHECK_EQ( L2.size(), 2 );
  i = 0;
  for (it = L1.begin(); it != L1.end(); ++it)
    UTST_CHECK_EQ( *it, ++i );
  for (it = L2.begin(); it != L2.end(); ++it)
    UTST_CHECK_EQ( *it, 1 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  test_assign();
  printf ("#### test_assign   Passed\n\n");
  test_insert();
  printf ("#### test_insert   Passed\n\n");
  test_erase();
  printf ("#### test_erase    Passed\n\n");
  test_splice();
  printf ("#### test_splice   Passed\n\n");
  return 0;
}
