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

#include "ListXcelHLS.h"

//--------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------
template<typename T>
inline list<T>::list()
  : m_node(new list_node(T()))
{
  m_node->m_next = m_node->m_prev = m_node;
}

template<typename T>
inline list<T>::list( size_type n, const T& value )
  : m_node(new list_node(T()))
{
  m_node->m_next = m_node->m_prev = m_node;
  insert(begin(), n, value);
}

template<typename T>
inline list<T>::list( iterator first, iterator last )
  : m_node(new list_node(T()))
{
  m_node->m_next = m_node->m_prev = m_node;
  insert(begin(), first, last);
}

template<typename T>
inline list<T>::list( const list& x )
  : m_node(new list_node(T()))
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
  delete m_node;
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
// Clear
//--------------------------------------------------------------------
template <typename T>
inline void list<T>::clear() {
  erase( begin(), end() );
}

#endif // POLYHS_LIST_INL
