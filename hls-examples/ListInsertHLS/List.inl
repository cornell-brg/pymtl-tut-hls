//========================================================================
// PolyHS list inline methods
//========================================================================
// Authors : Shreesha Srinath, Ritchie Zhao
// Date    : September 17, 2014
// Project : Polymorphic Hardware Specialization
//
// Linked list container implementation inspired from C++ STL.

#ifndef POLYHS_LIST_INL
#define POLYHS_LIST_INL

#include "List.h"

//--------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------
template<typename T>
inline list<T>::list()
  : m_node( get_node( T() ) )
{
  m_node->m_next = m_node->m_prev = m_node;
}

template<typename T>
inline list<T>::list( size_type n, const T& value )
  : m_node( get_node() )
{
  m_node->m_next = m_node->m_prev = m_node;
  insert(begin(), n, value);
}

template<typename T>
inline list<T>::list( iterator first, iterator last )
  : m_node( get_node() )
{
  m_node->m_next = m_node->m_prev = m_node;
  insert(begin(), first, last);
}

template<typename T>
inline list<T>::list( const list& x )
  : m_node( get_node() )
{
  m_node->m_next = m_node->m_prev = m_node;
  insert (begin(), x.begin(), x.end());
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
template<typename T>
inline list<T>::~list() {
  clear();
  put_node( m_node );
}

//--------------------------------------------------------------------
// size methods
//--------------------------------------------------------------------
template<typename T>
inline typename list<T>::size_type list<T>::size() const {
  size_type tmp = 0;
  for (const_iterator i = begin(); i != end(); ++i)
    tmp++;
  return tmp;
}

template<typename T>
inline bool list<T>::empty() const {
  return size() == 0;
}

//--------------------------------------------------------------------
// Iterator methods
//--------------------------------------------------------------------
template<typename T>
inline typename list<T>::const_iterator list<T>::begin() const {
  return const_iterator(m_node->m_next);
}

template<typename T>
inline typename list<T>::const_iterator list<T>::end() const {
  return const_iterator(m_node);
}

template<typename T>
inline typename list<T>::const_iterator list<T>::cbegin() const {
  return const_iterator(m_node->m_next);
}

template<typename T>
inline typename list<T>::const_iterator list<T>::cend() const {
  return const_iterator(m_node);
}

template<typename T>
inline typename list<T>::iterator list<T>::begin() {
  return iterator(m_node->m_next);
}

template<typename T>
inline typename list<T>::iterator list<T>::end() {
  return iterator(m_node);
}

//--------------------------------------------------------------------
// Push/Pop front
//--------------------------------------------------------------------
template <typename T>
inline void list<T>::push_front( const T& val ) {
  insert (begin(), val);
}

template <typename T>
inline void list<T>::pop_front() {
  erase (begin());
}

//--------------------------------------------------------------------
// Push/Pop back
//--------------------------------------------------------------------
template <typename T>
inline void list<T>::push_back( const T& val ) {
  insert (end(), val);
}

template <typename T>
inline void list<T>::pop_back() {
  iterator tmp = end();
  erase (--tmp);
}

//--------------------------------------------------------------------
// Insert
//--------------------------------------------------------------------
template <typename T>
typename list<T>::iterator list<T>::insert( const_iterator pos, const T& val ) {
  node_ptr new_node( get_node( val ) );
  new_node->m_next = pos.p;
  new_node->m_prev = pos.p->m_prev;
  
  (*(pos.p->m_prev)).m_next = new_node;
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
  node_ptr prev_node = pos.p->m_prev;
  node_ptr next_node = pos.p->m_next;
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
  node_ptr tmp = m_node;
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
// Clear
//--------------------------------------------------------------------
template <typename T>
inline void list<T>::clear() {
  erase( begin(), end() );
}
    
//--------------------------------------------------------------------
// Operator=
//--------------------------------------------------------------------
template <typename T>
list<T>& list<T>::operator=( const list& x ) {
  if (this != &x) {
    iterator first1 = begin();
    iterator last1  = end();
    const_iterator first2 = x.begin();
    const_iterator last2  = x.end();
    while (first1 != last1 && first2 != last2)
      *first1++ = *first2++;
    if (first2 == last2)
      erase(first1, last1);
    else
      insert(last1, first2, last2);
  }
  return *this;
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
  (*(last.p->m_prev)).m_next = pos.p;
  (*(first.p->m_prev)).m_next = last.p;
  (*(pos.p->m_prev)).m_next = first.p;

  node_ptr tmp = last.p->m_prev;
  last.p->m_prev = first.p->m_prev;
  first.p->m_prev = pos.p->m_prev;
  pos.p->m_prev = tmp;
}

template <typename T>
void list<T>::splice( const_iterator pos, list& x, const_iterator i ) {
  const_iterator i1 = i;
  splice( pos, x, i, ++i1 );
}

#endif // POLYHS_LIST_INL
