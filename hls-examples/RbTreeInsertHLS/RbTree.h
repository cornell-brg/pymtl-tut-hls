//========================================================================
// PolyHS rbtree
//========================================================================
// Author  : Ritchie Zhao
// Date    : July 17, 2015
// Project : Polymorphic Hardware Specialization
//
#ifndef POLYHS_RBTREE_H
#define POLYHS_RBTREE_H

#include <cstddef>
#include <iterator>
#include <assert.h>
#include "RbTreeProxy.h"

//----------------------------------------------------------------------
// tree rotation and recoloring
//----------------------------------------------------------------------
#define _NODE_PTR PointerProxy< _NodeProxy< _Key, _Value > >
#define _NODE_PTR_PROXY _NodePtrProxy< _NodeProxy< _Key, _Value > >
template<typename _Key, typename _Value>
void  _RbTreeRotateLeft ( _NODE_PTR x, _NODE_PTR_PROXY& root );
template<typename _Key, typename _Value>
void  _RbTreeRotateRight( _NODE_PTR x, _NODE_PTR_PROXY& root );
template<typename _Key, typename _Value>
void  _RbTreeRebalance  ( _NODE_PTR x, _NODE_PTR_PROXY& root );
template<typename _Key, typename _Value>
_NODE_PTR
_RbTreeRebalanceForErase( _NODE_PTR _z,              _NODE_PTR_PROXY& root,
                          _NODE_PTR_PROXY& leftmost, _NODE_PTR_PROXY& rightmost );
#undef _NODE_PTR
#undef _NODE_PTR_PROXY

//----------------------------------------------------------------------
// template _RbTreeIterator
//   Adds the dereference, ++, -- operators to the BaseIterator
//   Inherits the ==, != operators
//----------------------------------------------------------------------
template<class _Key, class _Value, class _KProxy, class _VProxy, class _NodePtr>
struct _RbTreeIterator {
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t                       difference_type;
  
  typedef _Value              value_type;
  typedef _VProxy             reference;
  typedef _KProxy             key_reference;
  typedef _VProxy             val_reference;
  typedef _NodePtr            pointer;
  typedef _RbTreeIterator<_Key, _Value, _KProxy, _VProxy, _NodePtr>
                              iterator;

  _NodePtr m_node;

  _RbTreeIterator(): m_node( 0 ) {}
  _RbTreeIterator( _NodePtr x ) : m_node( x ) {}
  _RbTreeIterator( const iterator& it ) : m_node( it.m_node ) {}

  void _increment();
  void _decrement();
  
  val_reference operator*() const { return m_node->m_value; }
  pointer  operator->() const { return m_node; }
  val_reference value() const { return m_node->m_value; }
  key_reference key() const { return m_node->m_key; }

  iterator& operator++() { _increment(); return *this; }
  iterator  operator++(int) {
    iterator temp = *this;
    _increment();
    return temp;
  }
  iterator& operator--() { _decrement(); return *this; }
  iterator  operator--(int) {
    iterator temp = *this;
    _decrement();
    return temp;
  }
};

//----------------------------------------------------------------------
// _RbTreeIterator equality operators
//----------------------------------------------------------------------
#define _RBTREE_ITER _RbTreeIterator<_Key,_Value,_KProxy,_VProxy,_NodePtr>
template<class _Key, class _Value, class _KProxy, class _VProxy, class _NodePtr>
inline bool operator==( const _RBTREE_ITER x, const _RBTREE_ITER y ) {
  return x.m_node == y.m_node;
}
template<class _Key, class _Value, class _KProxy, class _VProxy, class _NodePtr>
inline bool operator!=( const _RBTREE_ITER x, const _RBTREE_ITER y ) {
  return x.m_node != y.m_node;
}
#undef _RBTREE_ITER

//----------------------------------------------------------------------
// _RbTree template
//   The STL rbtree is further templated on the KeyComp and Allocator.
//   KeyComp is replaced by just key::operator==, and the Allocator
//   is just new and delete for now.
//
//   Both _Key and _Value are stored at a node, and an insert operation
//   must be done with both _Key and _Value.
//----------------------------------------------------------------------
template<class _Key, class _Value>
class _RbTree {
public:
  typedef _NodeProxy<_Key, _Value>            _Node;
  typedef PointerProxy< _NodeProxy<_Key,_Value> >
                                              _NodePtr;
  typedef _RbTreeColorType                    _ColorType;
protected:
  typedef _Key                                key_type;
  typedef _Value                              value_type;
  typedef size_t                              size_type;
  typedef ptrdiff_t                           difference_type;
  typedef _RbTree<_Key, _Value>               _Self;

public:
  // The header is used to quickly access the root node, leftmost node,
  // and rightmost node
  _NodePtr m_header;
  size_type m_node_count;

//----------------------------------------------------------------------
// Iterator Types
//----------------------------------------------------------------------
public:

  typedef _RbTreeIterator< _Key, _Value,
          ValueProxy<_Key>, ValueProxy<_Value>,
          _NodePtr > iterator;
  typedef _RbTreeIterator< _Key, _Value,
          const ValueProxy<_Key>, const ValueProxy<_Value>,
          _NodePtr > const_iterator;

//----------------------------------------------------------------------
// Allocation/Deallocation
//----------------------------------------------------------------------
protected:

  _NodePtr m_create_node( const key_type& k, const value_type& x ) {
    Address mem = (Address)malloc( _Node::size() );
    assert( mem != 0 );
    _NodePtr node( mem );
    node->m_key = k;
    node->m_value = x;
    return node;
  }
  void m_destroy_node( _NodePtr p ) {
    #ifdef CPP_COMPILE
    free( p.get_addr() );
    #endif
  }
  _NodePtr m_clone_node ( _NodePtr x );

//----------------------------------------------------------------------
// Basic Operations
//----------------------------------------------------------------------
public:

  _NodePtrProxy<_Node> m_root() const 
    { return m_header->m_parent; }
  _NodePtrProxy<_Node> m_leftmost() const 
    { return m_header->m_left; }
  _NodePtrProxy<_Node> m_rightmost() const 
    { return m_header->m_right; }

  static _NodePtrProxy<_Node>& s_left( _NodePtr x )
    { return (x->m_left); }
  static _NodePtrProxy<_Node>& s_right( _NodePtr x )
    { return (x->m_right); }
  static _NodePtrProxy<_Node>& s_parent( _NodePtr x )
    { return (x->m_parent); }
  static ValueProxy<_Value>& s_value( _NodePtr x )
    { return (x->m_value); }
  static const ValueProxy<_Key>& s_key( _NodePtr x )
    { return (x->m_key); }
  static ValueProxy<_ColorType>& s_color( _NodePtr x )
    { return (x->m_color); }

  static _NodePtr s_minimum( _NodePtr x ) {
    return _Node::s_minimum( x );
  }
  static _NodePtr s_maximum( _NodePtr x ) {
    return _Node::s_maximum( x );
  }

//----------------------------------------------------------------------
// Constructors / Destructors
//----------------------------------------------------------------------
public:

  _RbTree();
  _RbTree(const _Self& x);
  ~_RbTree();

//----------------------------------------------------------------------
// Operators
//   Just assignment for now
//----------------------------------------------------------------------

  _Self& operator=(const _Self& x);

//----------------------------------------------------------------------
// Private Helper Methods
//----------------------------------------------------------------------
private:

  iterator m_insert(_NodePtr x, _NodePtr y, const _Key& k, const _Value& v);
  _NodePtr m_copy(_NodePtr x, _NodePtr p);
  void m_erase(_NodePtr x);

//----------------------------------------------------------------------
// User Methods
//----------------------------------------------------------------------
public:    
                                // accessors:
  iterator begin() { return (_NodePtr)m_leftmost(); }
  const_iterator begin() const { return (_NodePtr)m_leftmost(); }
  iterator end() { return m_header; }
  const_iterator end() const { return m_header; }
  /*reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin());
  }*/
  bool empty() const { return m_node_count == 0; }
  size_type size() const { return m_node_count; }
  size_type max_size() const { return size_type(-1); }

  void swap(_Self& t) {
    std::swap(m_header, t.m_header);
    std::swap(m_node_count, t.m_node_count);
  }
    
public:
                                // insert/erase
  // RZ:  insert_equal not supported as it is not used by
  //      map or set
  std::pair<iterator,bool> insert_unique(
      const key_type& k, const value_type& x);
  iterator insert_unique(
      iterator position, const key_type& k, const value_type& x);
  void insert_unique(const_iterator first, const_iterator last);
  void insert_unique(iterator first, iterator last);
  //void insert_unique(const _Value* first, const _Value* last);
  //iterator insert_equal(const _Value& x);
  //iterator insert_equal(iterator position, const _Value& x);
  //void insert_equal(const_iterator first, const_iterator last);
  //void insert_equal(const _Value* first, const _Value* last);

  void erase(iterator position);
  size_type erase(const key_type& x);
  void erase(iterator first, iterator last);
  void erase(const key_type* first, const key_type* last);
  void empty_initialize();
  void clear();  

public:
                                // set operations:
  // finds iterator to x
  iterator find(const key_type& x);
  const_iterator find(const key_type& x) const;
  // counts number of elems with key equal to x (0 or 1)
  size_type count(const key_type& x) const;
  // returns an iterator to the first elem with key >= x 
  iterator lower_bound(const key_type& x);
  const_iterator lower_bound(const key_type& x) const;
  // returns an iterator to the first elem with key > x 
  iterator upper_bound(const key_type& x);
  const_iterator upper_bound(const key_type& x) const;
  
  // returns bounds of a range containing elems with key == x
  std::pair<iterator,iterator> equal_range(const key_type& x) {
    return std::pair<iterator, iterator>(
        lower_bound(x), upper_bound(x));
  }
  std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
    return std::pair<const_iterator,const_iterator>(
        lower_bound(x), upper_bound(x));
  }

public:
                                // Debugging.
  int black_count( _NodePtr node, _NodePtr root ) const;
  bool _rb_verify() const;
  void _dump_node( const _NodePtr node, const std::string prefix="", const char lr='x' ) const;
  void _dump_subtree( const _NodePtr node, const std::string prefix="", const char lr='x' ) const;
  void _dump_tree() const;

}; // end class _RbTree

#include "RbTree.inl"
#endif
