//========================================================================
// PolyHS rbtree implementation unit tests
//========================================================================

#include "RbTree.h"
#include <stdio.h>
#include <assert.h>

#define UTST_CHECK_EQ(x,y) assert((x)==(y))

struct select1st {
  template<typename K, typename V>
  const K& operator()( std::pair<K,V> const& p ) const {
    return p.first;
  }
};

typedef unsigned Key;
typedef unsigned Value;
typedef _RbTree<Key, Value> RbTree;

Key get_key( RbTree::iterator it ) { return it.key(); }
Value get_val( RbTree::iterator it ) { return *it; }

//------------------------------------------------------------------------
// Test Cons Simple
//------------------------------------------------------------------------
void test_cons_simple()
{
  RbTree tree;
  unsigned i;
  const unsigned N = 16;
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.empty(), true );

  for (i = 0; i < N; ++i) 
    tree.insert_unique( i+1,i+1 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  tree._dump_tree();

  i = 0;
  for ( RbTree::iterator it = tree.begin(); it != tree.end(); ++it ) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }
  UTST_CHECK_EQ( i, N );
}

//------------------------------------------------------------------------
// Test Cons
//------------------------------------------------------------------------
void test_cons()
{
  RbTree tree;
  unsigned i;
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.empty(), true );

  tree.insert_unique( 2,2 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  tree.insert_unique( 1,1 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  tree.insert_unique( 3,3 );
  UTST_CHECK_EQ( tree._rb_verify(), true );

  i = 0;
  for ( RbTree::iterator it = tree.begin(); it != tree.end(); ++it ) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }

  RbTree tree2( tree );
  UTST_CHECK_EQ( tree2._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), tree2.size() );

  RbTree::iterator it  = tree.begin();
  RbTree::iterator it2 = tree2.begin();
  while ( it != tree.end() && it2 != tree2.end() ) {
    UTST_CHECK_EQ( get_key(it),  get_key(it2) );
    UTST_CHECK_EQ( get_val(it), get_val(it2) );
    ++it;
    ++it2;
  }
}

//------------------------------------------------------------------------
// Test Insert
//------------------------------------------------------------------------
void test_insert()
{
  RbTree tree;
  RbTree::iterator it;
  unsigned i;

  // test insert_unique( const Value )
  tree.insert_unique( 2,2 );
  tree.insert_unique( 1,1 );
  tree.insert_unique( 3,3 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 3 );

  i = 0;
  for (it = tree.begin(); it != tree.end(); ++it) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }
  
  // test insert_unique( iterator pos, const Value )
  // (inserts with hint)
  tree.insert_unique( tree.begin(), 4,4 );
  tree.insert_unique( tree.end(), 5,5 );
  it = tree.begin(); ++it;
  tree.insert_unique( it, 6,6 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 6 );
  
  i = 0;
  for (it = tree.begin(); it != tree.end(); ++it) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }

  // test insert_unique( tree:const_iterator, tree::const_iterator )
  RbTree tree2;
  it = tree.begin();
  ++it;
  ++it;
  ++it;
  ++it;
  tree2.insert_unique( tree.begin(), it );
  tree.clear();
  UTST_CHECK_EQ( tree2._rb_verify(), true );
  UTST_CHECK_EQ( tree2.size(), 4 );

  // test insert_unique( Value*, Value* )
  /*Value test_array[2] = { 4,4, 3,3 };
  tree2.insert_unique( test_array, test_array+2 );
  UTST_CHECK_EQ( tree2._rb_verify(), true );
  UTST_CHECK_EQ( tree2.size(), 4 );*/
  
  i = 0;
  for (it = tree2.begin(); it != tree2.end(); ++it) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }
}

//------------------------------------------------------------------------
// Test Erase
//------------------------------------------------------------------------
void test_erase()
{
  RbTree tree;
  RbTree::iterator it;
  unsigned i;

  tree.insert_unique( 2,2 );
  tree.insert_unique( 1,1 );
  tree.insert_unique( 3,3 );
  tree.insert_unique( 0,0 );
  UTST_CHECK_EQ( tree.size(), 4 );

  // test tree::erase( iterator position )
  tree.erase( tree.begin() );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 3 );
  
  i = 0;
  for (it = tree.begin(); it != tree.end(); ++it) {
    UTST_CHECK_EQ( get_key(it), i+1 );
    UTST_CHECK_EQ( get_val(it), i+1 );
    ++i;
  }
  
  // test tree::erase( key_type x )
  tree.erase( 3 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 2 );
  tree.erase( 1 );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 1 );

  it = tree.begin();
  UTST_CHECK_EQ( get_key(it), 2 );
  UTST_CHECK_EQ( get_val(it), 2 );
  
  tree.erase( tree.begin(), tree.end() );
  UTST_CHECK_EQ( tree._rb_verify(), true );
  UTST_CHECK_EQ( tree.size(), 0 );
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  test_cons_simple();
  test_cons();
  test_insert();
  test_erase();
  
  printf ("Tests completed\n");
  return 0;
}
