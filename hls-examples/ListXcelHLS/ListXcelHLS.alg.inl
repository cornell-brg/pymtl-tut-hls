//========================================================================
// PolyHS list implementation
//========================================================================
// Authors : Shreesha Srinath, Ritchie Zhao
// Date    : September 17, 2014
// Project : Polymorphic Hardware Specialization
//
// Linked list container implementation inspired from C++ STL.

#ifndef POLYHS_LIST_ALG_INL
#define POLYHS_LIST_ALG_INL

#include "ListXcelHLS.h"

//--------------------------------------------------------------------
// Insert
//--------------------------------------------------------------------
template <typename T>
typename list<T>::iterator list<T>::insert( const_iterator pos, const T& val ) {
  NodePtrProxy<T> new_node( get_new_node() );
  new_node->m_next = pos.p;
  new_node->m_prev = pos.p->m_prev;
  pos.p->m_prev->m_next = new_node;
  pos.p->m_prev = new_node;
  return iterator(new_node);
}

template <typename T>
typename list<T>::iterator list<T>::insert( const_iterator pos, size_type n, const T& val ) {
  for (size_type i = 0; i < n; ++i) {
    pos = insert(pos, val);
  }
  return pos.p;
}

template <typename T>
typename list<T>::iterator list<T>::insert( const_iterator pos, const_iterator first, const_iterator last ) {
  for (; first != last; ) {
    --last;
    pos = insert(pos, *last);
  }
  return pos.p;
}

//--------------------------------------------------------------------
// Erase
//--------------------------------------------------------------------
template <typename T>
typename list<T>::iterator list<T>::erase( const_iterator pos) {
  NodePtrProxy<T> prev_node = pos.p->m_prev;
  NodePtrProxy<T> next_node = pos.p->m_next;
  prev_node->m_next = next_node;
  next_node->m_prev = prev_node;
  put_node( pos.p );
  return iterator(next_node);
}

template <typename T>
typename list<T>::iterator list<T>::erase( const_iterator first, const_iterator last ) {
  for (; first != last; ) {
    first = erase(first);
  }
  return iterator(last.p);
}

//--------------------------------------------------------------------
// Swap
//--------------------------------------------------------------------
template <typename T>
void list<T>::swap( list& x ) {
  NodePtrProxy<T> tmp = m_node;
  m_node = x.m_node;
  x.m_node = tmp;
}

//--------------------------------------------------------------------
// Resize
//--------------------------------------------------------------------
template <typename T>
void list<T>::resize( size_type n, const T& val ) {
  const_iterator it = begin();
  size_type len = 0;
  for (; it != end() && len < n; ++it,++n)
    ;
  if (len == n)
    erase(it, end());
  else
    insert(end(), n-len, val);
}

//--------------------------------------------------------------------
// Splice
//--------------------------------------------------------------------
template <typename T>
void list<T>::splice( const_iterator pos, list& x ) {
  splice( pos, x, x.begin(), x.end() );
}

template <typename T>
void list<T>::splice( const_iterator pos, list& x, const_iterator first, const_iterator last ) {
  if (first == last) return;
  last.p->m_prev->m_next = pos.p;
  first.p->m_prev->m_next = last.p;
  pos.p->m_prev->m_next = first.p;

  list_node* tmp = last.p->m_prev;
  last.p->m_prev = first.p->m_prev;
  first.p->m_prev = pos.p->m_prev;
  pos.p->m_prev = tmp;
}

template <typename T>
void list<T>::splice( const_iterator pos, list& x, const_iterator i ) {
  const_iterator i1 = i;
  splice( pos, x, i, ++i1 );
}

#endif // POLYHS_LIST_CC
