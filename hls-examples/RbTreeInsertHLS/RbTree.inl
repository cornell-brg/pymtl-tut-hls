//========================================================================
// PolyHS rbtree inline methods
//========================================================================
// Authors : Shreesha Srinath, Ritchie Zhao
// Date    : July 19, 2015
// Project : Polymorphic Hardware Specialization
//
// rbtree inspired from C++ STL.

#ifndef POLYHS_RBTREE_INL
#define POLYHS_RBTREE_INL

#include <memory>
#include <assert.h>
#include "RbTree.h"
#include "Common.h"

// Macros for the sake of more readable code
#define _RB_TREE _RbTree<_Key, _Value>
#define _NODE_PTR PointerProxy< _NodeProxy< _Key, _Value > >
#define _NODE_PTR_PROXY _NodePtrProxy< _NodeProxy< _Key, _Value > >
#define _RBTREE_ITER _RbTreeIterator<_Key,_Value,_KProxy,_VProxy,_NodePtr>

//----------------------------------------------------------------------
// Memory Allocation/Deallocation
//----------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::_NodePtr
_RB_TREE::m_clone_node( _NodePtr x ) {
  _NodePtr tmp = m_create_node(x->m_key, x->m_value);
  tmp->m_color = x->m_color;
  tmp->m_left = 0;
  tmp->m_right = 0;
  return tmp;
}

//--------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------
template<class _Key, class _Value>
_RB_TREE::_RbTree()
  : m_header( m_create_node( _Key(), _Value() ) ), m_node_count(0)
{
  empty_initialize();
}

template<class _Key, class _Value>
_RB_TREE::_RbTree(const _RB_TREE& x) 
  : m_header( m_create_node( _Key(), _Value() ) ), m_node_count(0)
{
  if (x.m_root() == 0)
    empty_initialize();
  else {
    s_color(m_header) = s_RbTreeRed;
    m_root() = m_copy(x.m_root(), m_header);
    m_leftmost() = s_minimum(m_root());
    m_rightmost() = s_maximum(m_root());
  }
  m_node_count = x.m_node_count;
}

//--------------------------------------------------------------------
// Destructors
//--------------------------------------------------------------------
template<class _Key, class _Value>
_RB_TREE::~_RbTree() {
  clear();
  m_destroy_node( m_header );
}

//--------------------------------------------------------------------
// Operators
//--------------------------------------------------------------------
template<class _Key, class _Value>
_RB_TREE& _RB_TREE::operator=( const _RB_TREE& x ) {
  if( this != &x ) {
    clear();
    m_node_count = 0;
    if (x.m_root() == 0) {
      m_root() = 0;
      m_leftmost() = m_header;
      m_rightmost() = m_header;
    }
    else {
      m_root() = m_copy(x.m_root(), m_header);
      m_leftmost() = s_minimum(m_root());
      m_rightmost() = s_maximum(m_root());
      m_node_count = x.m_node_count;
    }
  }
  return *this;
}

//--------------------------------------------------------------------
// Private Helper Functions
//--------------------------------------------------------------------
// m_insert
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::iterator 
_RB_TREE::m_insert( _NodePtr x_, _NodePtr y_, const _Key& k, const _Value& v ) {
  _NodePtr x = (_NodePtr) x_;
  _NodePtr y = (_NodePtr) y_;
  _NodePtr z;

  if (y == m_header || x != 0 || 
      (k < s_key(y))) {
    z = m_create_node(k,v);
    s_left(y) = z;               // also makes m_leftmost() = z 
                                      //    when y == m_header
    if (y == m_header) {
      m_root() = z;
      m_rightmost() = z;
    }
    else if (y == m_leftmost())
      m_leftmost() = z;   // maintain m_leftmost() pointing to min node
  }
  else {
    z = m_create_node(k,v);
    s_right(y) = z;
    if (y == m_rightmost())
      m_rightmost() = z;  // maintain m_rightmost() pointing to max node
  }
  s_parent(z) = y;
  s_left(z) = 0;
  s_right(z) = 0;
  _RbTreeRebalance(z, m_header->m_parent);
  ++m_node_count;
  return iterator(z);
}

//--------------------------------------------------------------------
// m_copy
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::_NodePtr
_RB_TREE::m_copy( _NodePtr x, _NodePtr p ) {
  // structural copy.  x and p must be non-null.
  _NodePtr top = m_clone_node(x);
  top->m_parent = p;
 
  if (x->m_right != 0)
    top->m_right = m_copy(s_right(x), top);
  p = top;
  x = s_left(x);

  while (x != 0) {
    _NodePtr y = m_clone_node(x);
    p->m_left = y;
    y->m_parent = p;
    if (x->m_right != 0)
      y->m_right = m_copy(s_right(x), y);
    p = y;
    x = s_left(x);
  }

  return top;
}

//--------------------------------------------------------------------
// m_erase
//--------------------------------------------------------------------
template<class _Key, class _Value>
void _RB_TREE::m_erase( _NodePtr x ) {
  // erase without rebalancing
  while (x != 0) {
    m_erase(s_right(x));
    _NodePtr y = s_left(x);
     m_destroy_node(x);
    x = y;
  }
}

//--------------------------------------------------------------------
// empty_initialize and clear
//--------------------------------------------------------------------
template<class _Key, class _Value>
void _RB_TREE::empty_initialize() {
  s_color(m_header) = s_RbTreeRed; // used to distinguish header from 
                                 // root, in iterator.operator++
  m_root() = 0;
  m_leftmost() = m_header;
  m_rightmost() = m_header;

  //m_header->m_left = m_header;
  //m_header->m_right = m_header;
}

template<class _Key, class _Value>
void _RB_TREE::clear() {
  if (m_node_count != 0) {
    m_erase(m_root());
    m_leftmost() = m_header;
    m_root() = 0;
    m_rightmost() = m_header;
    m_node_count = 0;
  }
}      
  
//--------------------------------------------------------------------
// insert_unique
//--------------------------------------------------------------------
template<class _Key, class _Value>
std::pair<typename _RB_TREE::iterator, bool>
_RB_TREE::insert_unique( const _Key& _k, const _Value& _v ) {
  _NodePtr y = m_header;
  _NodePtr x = m_root();
  bool comp = true;
  while (x != 0) {
    y = x;
    comp = (_k < s_key(x));
    x = comp ? s_left(x) : s_right(x);
  }
  iterator j = iterator(y);   
  if (comp) {
    if (j == begin())     
      return std::pair<iterator,bool>(m_insert(x, y, _k, _v), true);
    else
      --j;
  }
  if (s_key(j.m_node) < _k)
    return std::pair<iterator,bool>(m_insert(x, y, _k, _v), true);
  return std::pair<iterator,bool>(j, false);
}

template<class _Key, class _Value>
typename _RB_TREE::iterator
_RB_TREE::insert_unique( iterator position, const _Key& _k, const _Value& _v )
{
  if (position.m_node == m_header->m_left) { // begin()
    if (size() > 0 && _k < s_key(position.m_node))
      return m_insert(position.m_node, position.m_node, _k, _v);
    // first argument just needs to be non-null 
    else
      return insert_unique(_k,_v).first;
  } else if (position.m_node == m_header) { // end()
    if (s_key(m_rightmost()) < _k)
      return m_insert(0, m_rightmost(), _k, _v);
    else
      return insert_unique(_k,_v).first;
  } else {
    iterator before = position;
    --before;
    if (s_key(before.m_node) < _k 
        && _k < s_key(position.m_node)) {
      if (s_right(before.m_node) == 0)
        return m_insert(0, before.m_node, _k, _v); 
      else
        return m_insert(position.m_node, position.m_node, _k, _v);
    // first argument just needs to be non-null 
    } else
      return insert_unique(_k,_v).first;
  }
}

template<class _Key, class _Value>
void _RB_TREE::insert_unique( const_iterator first, const_iterator last )
{
  for ( ; first != last; ++first)
    insert_unique(first.key(), *first);
}

  template<class _Key, class _Value>
void _RB_TREE::insert_unique( iterator first, iterator last )
{
  for ( ; first != last; ++first)
    insert_unique(first.key(), *first);
}

/*template<class _Key, class _Value>
void _RB_TREE::insert_unique( const _Value* first, const _Value* last )
{
  for ( ; first != last; ++first)
    insert_unique(*first);
}*/

//--------------------------------------------------------------------
// erase
//--------------------------------------------------------------------
template<class _Key, class _Value>
void _RB_TREE::erase( iterator position ) {
  _NodePtr y = 
    (_NodePtr) _RbTreeRebalanceForErase(
                  position.m_node,
                  m_header->m_parent,
                  m_header->m_left,
                  m_header->m_right);
  m_destroy_node(y);
  --m_node_count;
}

template<class _Key, class _Value>
typename _RB_TREE::size_type _RB_TREE::erase( const key_type& x ) {
  std::pair<iterator,iterator> _p = equal_range(x);
  size_type _n = std::distance(_p.first, _p.second);
  erase(_p.first, _p.second);
  return _n;
}

template<class _Key, class _Value>
void _RB_TREE::erase( iterator first, iterator last ) {
  if (first == begin() && last == end())
    clear();
  else
    while (first != last) erase(first++);
}

template<class _Key, class _Value>
void _RB_TREE::erase( const key_type* first, const key_type* last ) {
  while (first != last) erase(*first++);
}

//--------------------------------------------------------------------
// find
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::iterator
_RB_TREE::find(const key_type& _k)
{
  _NodePtr y = m_header;      // Last node which is not less than _k. 
  _NodePtr x = m_root();      // Current node. 

  while (x != 0) 
    if (!(s_key(x) < _k))
      y = x, x = s_left(x);
    else
      x = s_right(x);

  iterator j = iterator(y);   
  return (j == end() || (_k < s_key(j.m_node))) ? 
     end() : j;
}

template<class _Key, class _Value>
typename _RB_TREE::const_iterator
_RB_TREE::find(const key_type& _k) const
{
  _NodePtr y = m_header; /* Last node which is not less than _k. */
  _NodePtr x = m_root(); /* Current node. */

  while (x != 0) {
    if (!(s_key(x) < _k))
      y = x, x = s_left(x);
    else
      x = s_right(x);
  }
  const_iterator j = const_iterator(y);   
  return (j == end() || (_k < s_key(j.m_node))) ?
    end() : j;
}

//--------------------------------------------------------------------
// count
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::size_type
_RB_TREE::count(const key_type& _k) const
{
  std::pair<const_iterator, const_iterator> p = equal_range(_k);
  size_type n = 0;
  std::distance(p.first, p.second, n);
  return n;
}

//--------------------------------------------------------------------
// lower_bound
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::iterator
_RB_TREE::lower_bound(const key_type& _k)
{
  _NodePtr y = m_header; /* Last node which is not less than _k. */
  _NodePtr x = m_root(); /* Current node. */

  while (x != 0) 
    if (!(s_key(x) < _k))
      y = x, x = s_left(x);
    else
      x = s_right(x);

  return iterator(y);
}

template<class _Key, class _Value>
typename _RB_TREE::const_iterator
_RB_TREE::lower_bound(const key_type& _k) const
{
  _NodePtr y = m_header; /* Last node which is not less than _k. */
  _NodePtr x = m_root(); /* Current node. */

  while (x != 0) 
    if (!(s_key(x) < _k))
      y = x, x = s_left(x);
    else
      x = s_right(x);

  return const_iterator(y);
}

//--------------------------------------------------------------------
// upper_bound
//--------------------------------------------------------------------
template<class _Key, class _Value>
typename _RB_TREE::iterator
_RB_TREE::upper_bound(const key_type& _k)
{
  _NodePtr y = m_header; /* Last node which is greater than _k. */
  _NodePtr x = m_root(); /* Current node. */

   while (x != 0) 
     if (_k < s_key(x))
       y = x, x = s_left(x);
     else
       x = s_right(x);

   return iterator(y);
}

template<class _Key, class _Value>
typename _RB_TREE::const_iterator
_RB_TREE::upper_bound(const key_type& _k) const
{
  _NodePtr y = m_header; /* Last node which is greater than _k. */
  _NodePtr x = m_root(); /* Current node. */

   while (x != 0) 
     if (_k < s_key(x))
       y = x, x = s_left(x);
     else
       x = s_right(x);

   return const_iterator(y);
}

//--------------------------------------------------------------------
// Debugging
//--------------------------------------------------------------------
template<class _Key, class _Value>
int _RB_TREE::black_count(_NodePtr node, _NodePtr root) const
{
  if (node == 0)
    return 0;
  else {
    int bc = node->m_color == s_RbTreeBlack ? 1 : 0;
    if (node == root)
      return bc;
    else
      return bc + black_count(node->m_parent, root);
  }
}

template<class _Key, class _Value>
bool _RB_TREE::_rb_verify() const {
  if (m_node_count == 0 || begin() == end())
    return m_node_count == 0 && begin() == end() &&
      m_header->m_left == m_header && m_header->m_right == m_header;
  
  int len = black_count(m_leftmost(), m_root());
  for (const_iterator __it = begin(); __it != end(); ++__it) {
    _NodePtr x = (_NodePtr) __it.m_node;
    _NodePtr L = s_left(x);
    _NodePtr R = s_right(x);

    if (x->m_color == s_RbTreeRed)
      if ((L && L->m_color == s_RbTreeRed) ||
          (R && R->m_color == s_RbTreeRed))
        return false;

    if (L && (s_key(x) < s_key(L)))
      return false;
    if (R && (s_key(R) < s_key(x)))
      return false;

    if (!L && !R && black_count(x, m_root()) != len)
      return false;
  }

  if (m_leftmost() != _RB_TREE::s_minimum(m_root()))
    return false;
  if (m_rightmost() != _RB_TREE::s_maximum(m_root()))
    return false;

  return true;
}

//----------------------------------------------------------------------
// _dump_node and _dump_tree
//----------------------------------------------------------------------
template<class _Key, class _Value>
void _RB_TREE::_dump_node( const _NodePtr node,
                           const std::string prefix,
                           const char lr ) const {
  if (node == 0) {
    printf ("%s%c\n", prefix.c_str(), lr);
    return;
  }
  printf ("%s%c:(%3d,%3d):%s\n", prefix.c_str(), lr,
      (_Value)node->m_key, (_Value)node->m_value,
      ((_ColorType)node->m_color ? "blk" : "red")
    );
}

template<class _Key, class _Value>
void _RB_TREE::_dump_subtree( const _NodePtr node,
                              const std::string prefix,
                              const char lr ) const {
  _dump_node( node, prefix, lr );
  if (node->m_left != 0)
    _dump_subtree( node->m_left,  prefix+"  ", 'L' );
  if (node->m_right != 0)
    _dump_subtree( node->m_right, prefix+"  ", 'R' );
}

template<class _Key, class _Value>
void _RB_TREE::_dump_tree() const {

  if( m_root() != 0) {
    _dump_node( m_root(), "Rt: " );
  }
  if( m_leftmost () != 0) {
    _dump_node( m_leftmost (), "Min:" );
  }
  if( m_rightmost() != 0) {
    _dump_node( m_rightmost(), "Max:" );
  }

  _dump_subtree( m_root() );
}

//----------------------------------------------------------------------
// _RbTreeIterator increment
//----------------------------------------------------------------------
template<class _Key, class _Value,
         class _KProxy, class _VProxy, class _NodePtr>
void _RBTREE_ITER::_increment() {
  if( m_node->m_right != 0 ) {
    m_node = m_node->m_right;
    while( m_node->m_left != 0 ) {
      m_node = m_node->m_left;
    }
  }
  else {
    _NodePtr y = m_node->m_parent;
    while( m_node == y->m_right ) {
      m_node = y;
      y = y->m_parent;
    }
    if( m_node->m_right != y )
      m_node = y;
  }
}

//----------------------------------------------------------------------
// _RbTreeIterator decrement
//----------------------------------------------------------------------
template<class _Key, class _Value,
         class _KProxy, class _VProxy, class _NodePtr>
void _RBTREE_ITER::_decrement() {
  if( m_node->m_color == s_RbTreeRed && (*(m_node->m_parent)).m_parent == m_node) {
    m_node = m_node->m_right;
  }
  else if( m_node-> m_left != 0 ) {
    _NodePtr y = m_node->m_left;
    while( y->m_right != 0 )
      y = y->m_right;
    m_node = y;
  }
  else {
    _NodePtr y = m_node->m_parent;
    while( m_node == y->m_left ) {
      m_node = y;
      y = y->m_parent;
    }
    m_node = y;
  }
}

//----------------------------------------------------------------------
// Tree rotation and recoloring using the base node pointers
// Note these were inline in the original stl impl.
//----------------------------------------------------------------------
// Rotate Left
//----------------------------------------------------------------------
template<typename _Key, typename _Value>
void _RbTreeRotateLeft( _NODE_PTR x, _NODE_PTR_PROXY& root ) {
  _NODE_PTR y = x->m_right;
  x->m_right = y->m_left;
  if( y->m_left != 0 )
    y->m_left->m_parent = x;
  y->m_parent = x->m_parent;

  if (x == root)
    root = y;
  else if (x == x->m_parent->m_left)
    x->m_parent->m_left = y;
  else
    x->m_parent->m_right = y;
  y->m_left = x;
  x->m_parent = y;
}

//----------------------------------------------------------------------
// Rotate Right
//----------------------------------------------------------------------
template<typename _Key, typename _Value>
void _RbTreeRotateRight( _NODE_PTR x, _NODE_PTR_PROXY& root ) {
  _NODE_PTR y = x->m_left;
  x->m_left = y->m_right;
  if (y->m_right != 0)
    y->m_right->m_parent = x;
  y->m_parent = x->m_parent;

  if (x == root)
    root = y;
  else if (x == x->m_parent->m_right)
    x->m_parent->m_right = y;
  else
    x->m_parent->m_left = y;
  y->m_right = x;
  x->m_parent = y;
}

//----------------------------------------------------------------------
// Rebalance
//----------------------------------------------------------------------
template<typename _Key, typename _Value>
void _RbTreeRebalance( _NODE_PTR x, _NODE_PTR_PROXY& root )
{
  DB_ASSERT(( x != 0 ));
  DB_ASSERT(( root != 0 ));
  x->m_color = s_RbTreeRed;
  while (x != root && x->m_parent->m_color == s_RbTreeRed) {
    if (x->m_parent == x->m_parent->m_parent->m_left) {
      _NODE_PTR y = x->m_parent->m_parent->m_right;
      //XXX
      //if (y && y->m_color == s_RbTreeRed) {
      if (y != 0 && y->m_color == s_RbTreeRed) {
        x->m_parent->m_color = s_RbTreeBlack;
        y->m_color = s_RbTreeBlack;
        x->m_parent->m_parent->m_color = s_RbTreeRed;
        x = x->m_parent->m_parent;
      }
      else {
        if (x == x->m_parent->m_right) {
          x = x->m_parent;
          _RbTreeRotateLeft(x, root);
        }
        x->m_parent->m_color = s_RbTreeBlack;
        x->m_parent->m_parent->m_color = s_RbTreeRed;
        _RbTreeRotateRight((_NODE_PTR)x->m_parent->m_parent, root);
      }
    }
    else {
      _NODE_PTR y = x->m_parent->m_parent->m_left;
      //XXX
      //if (y 0 && y->m_color == s_RbTreeRed) {
      if (y != 0 && y->m_color == s_RbTreeRed) {
        x->m_parent->m_color = s_RbTreeBlack;
        y->m_color = s_RbTreeBlack;
        x->m_parent->m_parent->m_color = s_RbTreeRed;
        x = x->m_parent->m_parent;
      }
      else {
        if (x == x->m_parent->m_left) {
          x = x->m_parent;
          _RbTreeRotateRight(x, root);
        }
        x->m_parent->m_color = s_RbTreeBlack;
        x->m_parent->m_parent->m_color = s_RbTreeRed;
        _RbTreeRotateLeft((_NODE_PTR)x->m_parent->m_parent, root);
      }
    }
  }
  root->m_color = s_RbTreeBlack;
} // end _RbTreeRebalance

//----------------------------------------------------------------------
// Rebalance for Erase
//----------------------------------------------------------------------
template<typename _Key, typename _Value>
_NODE_PTR
_RbTreeRebalanceForErase( _NODE_PTR _z,
                          _NODE_PTR_PROXY& root,
                          _NODE_PTR_PROXY& leftmost,
                          _NODE_PTR_PROXY& rightmost)
{
  _NODE_PTR y = _z;
  _NODE_PTR x = 0;
  _NODE_PTR x_parent = 0;
  if (y->m_left == 0)     // _z has at most one non-null child. y == z.
    x = y->m_right;     // x might be null.
  else
    if (y->m_right == 0)  // _z has exactly one non-null child. y == z.
      x = y->m_left;    // x is not null.
    else {                   // _z has two non-null children.  Set y to
      y = y->m_right;   //   _z's successor.  x might be null.
      while (y->m_left != 0)
        y = y->m_left;
      x = y->m_right;
    }
  if (y != _z) {          // relink y in place of z.  y is z's successor
    _z->m_left->m_parent = y;
    y->m_left = _z->m_left;
    if (y != _z->m_right) {
      x_parent = y->m_parent;
      if (x) x->m_parent = y->m_parent;
      y->m_parent->m_left = x;      // y must be a child of m_left
      y->m_right = _z->m_right;
      _z->m_right->m_parent = y;
    }
    else
      x_parent = y;  
    if (root == _z)
      root = y;
    else if (_z->m_parent->m_left == _z)
      _z->m_parent->m_left = y;
    else 
      _z->m_parent->m_right = y;
    y->m_parent = _z->m_parent;
    std::swap(y->m_color, _z->m_color);
    y = _z;
    // y now points to node to be actually deleted
  }
  else {                        // y == _z
    x_parent = y->m_parent;
    if (x) x->m_parent = y->m_parent;   
    if (root == _z)
      root = x;
    else 
      if (_z->m_parent->m_left == _z)
        _z->m_parent->m_left = x;
      else
        _z->m_parent->m_right = x;
    if (leftmost == _z) {
      if (_z->m_right == 0)        // _z->m_left must be null also
        leftmost = _z->m_parent;
    // makes leftmost == m_header if _z == root
      else
        leftmost = _NodeProxy<_Key,_Value>::s_minimum(x);
    }
    if (rightmost == _z) {
      if (_z->m_left == 0)         // _z->m_right must be null also
        rightmost = _z->m_parent;  
    // makes rightmost == m_header if _z == root
      else                      // x == _z->m_left
        rightmost = _NodeProxy<_Key,_Value>::s_maximum(x);
    }
  }
  if (y->m_color != s_RbTreeRed) { 
    while (x != root && (x == 0 || x->m_color == s_RbTreeBlack))
      if (x == x_parent->m_left) {
        _NODE_PTR w = x_parent->m_right;
        if (w->m_color == s_RbTreeRed) {
          w->m_color = s_RbTreeBlack;
          x_parent->m_color = s_RbTreeRed;
          _RbTreeRotateLeft(x_parent, root);
          w = x_parent->m_right;
        }
        if ((w->m_left == 0 || 
             w->m_left->m_color == s_RbTreeBlack) &&
            (w->m_right == 0 || 
             w->m_right->m_color == s_RbTreeBlack)) {
          w->m_color = s_RbTreeRed;
          x = x_parent;
          x_parent = x_parent->m_parent;
        } else {
          if (w->m_right == 0 || 
              w->m_right->m_color == s_RbTreeBlack) {
            if (w->m_left != 0) w->m_left->m_color = s_RbTreeBlack;
            w->m_color = s_RbTreeRed;
            _RbTreeRotateRight(w, root);
            w = x_parent->m_right;
          }
          w->m_color = x_parent->m_color;
          x_parent->m_color = s_RbTreeBlack;
          if (w->m_right != 0) w->m_right->m_color = s_RbTreeBlack;
          _RbTreeRotateLeft(x_parent, root);
          break;
        }
      } else {                  // same as above, with m_right <-> m_left.
        _NODE_PTR w = x_parent->m_left;
        if (w->m_color == s_RbTreeRed) {
          w->m_color = s_RbTreeBlack;
          x_parent->m_color = s_RbTreeRed;
          _RbTreeRotateRight(x_parent, root);
          w = x_parent->m_left;
        }
        if ((w->m_right == 0 || 
             w->m_right->m_color == s_RbTreeBlack) &&
            (w->m_left == 0 || 
             w->m_left->m_color == s_RbTreeBlack)) {
          w->m_color = s_RbTreeRed;
          x = x_parent;
          x_parent = x_parent->m_parent;
        } else {
          if (w->m_left == 0 || 
              w->m_left->m_color == s_RbTreeBlack) {
            if (w->m_right != 0) w->m_right->m_color = s_RbTreeBlack;
            w->m_color = s_RbTreeRed;
            _RbTreeRotateLeft(w, root);
            w = x_parent->m_left;
          }
          w->m_color = x_parent->m_color;
          x_parent->m_color = s_RbTreeBlack;
          if (w->m_left != 0) w->m_left->m_color = s_RbTreeBlack;
          _RbTreeRotateRight(x_parent, root);
          break;
        }
      }
    if (x) x->m_color = s_RbTreeBlack;
  }
  return y;
} // end RbTreeRebalanceForErase

#undef _RB_TREE
#undef _NODE_PTR
#undef _NODE_PTR_PROXY
#undef _RBTREE_ITER

#endif // POLYHSRBTREE_INL
