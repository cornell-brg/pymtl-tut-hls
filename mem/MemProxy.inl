//========================================================================
// MemProxy
//========================================================================

namespace mem {

  //----------------------------------------------------------------------
  // MemProxy<T>: Constructor
  //----------------------------------------------------------------------

  template < typename T >
  MemProxy<T>::MemProxy( unsigned int addr )
    : m_addr(addr), m_memoized_valid(false)
  { }

  //----------------------------------------------------------------------
  // MemProxy<T>: Cast Operator
  //----------------------------------------------------------------------

  template < typename T >
  MemProxy<T>::operator T() const
  {
    #pragma HLS inline
    if ( !m_memoized_valid ) {
      m_memoized_valid = true;
      mem::InMemStream is(m_addr);
      is >> m_memoized_value;
    }
    return m_memoized_value;
  }

  //----------------------------------------------------------------------
  // MemProxy<T>: Assignment Operators
  //----------------------------------------------------------------------

  template < typename T >
  MemProxy<T>& MemProxy<T>::operator=( const T& value )
  {
    #pragma HLS inline
    m_memoized_value = value;
    m_memoized_valid = true;
    mem::OutMemStream os(m_addr);
    os << m_memoized_value;
    return *this;
  }

  template < typename T >
  MemProxy<T>& MemProxy<T>::operator=( const MemProxy<T>& rhs )
  {
    #pragma HLS inline
    m_memoized_value = static_cast<T>(rhs);
    m_memoized_valid = true;
    mem::OutMemStream os(m_addr);
    os << m_memoized_value;
    return *this;
  }

  //----------------------------------------------------------------------
  // MemProxy<T>: Address Operators
  //----------------------------------------------------------------------

  template < typename T >
  MemPointer<T> MemProxy<T>::operator&()
  {
    return MemPointer<T>(m_addr);
  }

  template < typename T >
  const MemPointer<T> MemProxy<T>::operator&() const
  {
    return MemPointer<T>(m_addr);
  }

  //----------------------------------------------------------------------
  // MemProxy<MemPointer<U>>: Constructor
  //----------------------------------------------------------------------

  template < typename U >
  MemProxy< MemPointer<U> >::MemProxy( unsigned int addr )
    : m_addr(addr)
  { }

  //----------------------------------------------------------------------
  // MemProxy<MemPointer<U>>: Cast Operator
  //----------------------------------------------------------------------

  template < typename U >
  MemProxy< MemPointer<U> >::operator MemPointer<U>() const
  {
    MemPointer<U> ptr;
    mem::InMemStream is(m_addr);
    is >> ptr;
    return ptr;
  }

  //----------------------------------------------------------------------
  // MemProxy<MemPointer<U>>: Assignment Operators
  //----------------------------------------------------------------------

  template < typename U >
  MemProxy< MemPointer<U> >&
  MemProxy< MemPointer<U> >::operator=( unsigned int value )
  {
    mem::OutMemStream os(m_addr);
    os << value;
    return *this;
  }

  template < typename U >
  MemProxy< MemPointer<U> >&
  MemProxy< MemPointer<U> >::operator=( const MemPointer<U>& value )
  {
    mem::OutMemStream os(m_addr);
    os << value;
    return *this;
  }

  template < typename U >
  MemProxy< MemPointer<U> >&
  MemProxy< MemPointer<U> >::operator=( const MemProxy< MemPointer<U> >& rhs )
  {
    mem::OutMemStream os(m_addr);
    os << static_cast< MemPointer<U> >(rhs);
    return *this;
  }

  //----------------------------------------------------------------------
  // MemProxy<MemPointer<U>>: Dereference Operator
  //----------------------------------------------------------------------

  template < typename U >
  MemProxy<U>
  MemProxy< MemPointer<U> >::operator*()
  {
    unsigned int addr;
    mem::InMemStream is(m_addr);
    is >> addr;
    return MemProxy<U>(addr);
  }

  //----------------------------------------------------------------------
  // MemPointer<T>: Constructors
  //----------------------------------------------------------------------

  template < typename T >
  MemPointer<T>::MemPointer()
    : m_addr(0)
  { }

  template < typename T >
  MemPointer<T>::MemPointer( unsigned int addr )
    : m_addr(addr)
  { }

  //----------------------------------------------------------------------
  // MemPointer<T>: Assignment Operators
  //----------------------------------------------------------------------

  template < typename T >
  MemPointer<T>& MemPointer<T>::operator=( unsigned int value )
  {
    m_addr = value;
    return *this;
  }

  template < typename T >
  MemPointer<T>& MemPointer<T>::operator=( const MemPointer<T>& rhs )
  {
    m_addr = rhs.m_addr;
    return *this;
  }

  //----------------------------------------------------------------------
  // MemPointer<T>: Comparison Operators
  //----------------------------------------------------------------------

  template < typename T >
  bool MemPointer<T>::operator==( int addr ) const
  {
    return ( m_addr == addr );
  }

  template < typename T >
  bool MemPointer<T>::operator==( const MemPointer<T>& rhs ) const
  {
    return ( m_addr == rhs.m_addr );
  }

  template < typename T >
  bool MemPointer<T>::operator!=( int addr ) const
  {
    return ( m_addr != addr );
  }

  template < typename T >
  bool MemPointer<T>::operator!=( const MemPointer<T>& rhs ) const
  {
    return ( m_addr != rhs.m_addr );
  }

  //----------------------------------------------------------------------
  // MemPointer<T>: Dereference Operator
  //----------------------------------------------------------------------

  template < typename T >
  MemProxy<T> MemPointer<T>::operator*()
  {
    return MemProxy<T>(m_addr);
  }

  // Will need magic here to make & return a real pointer for a proxy.

  /*
  T* operator->()
  {
    assert( m_addr != 0 );
    mem::InMemStream is(m_addr);
    is >> m_temp_value;
    return &m_temp_value;
  }
  */

  //----------------------------------------------------------------------
  // MemPointer<T>: Insertion operator overloading
  //----------------------------------------------------------------------

  template < typename T >
  OutMemStream& operator<<( OutMemStream& os, const MemPointer<T>& rhs )
  {
    os << rhs.m_addr;
    return os;
  }

  //----------------------------------------------------------------------
  // MemPointer<T>: Extraction operator overloading
  //----------------------------------------------------------------------

  template < typename T >
  InMemStream& operator>>( InMemStream& is, MemPointer<T>& rhs )
  {
    is >> rhs.m_addr;
    return is;
  }

}

