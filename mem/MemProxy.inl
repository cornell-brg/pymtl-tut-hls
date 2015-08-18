//==============================================================
// generic proxies inlines
//==============================================================

namespace mem {

//=================================================================
//=================================================================
// MemProxy
//=================================================================
//=================================================================

  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  template<typename T>
  MemProxy<T>::MemProxy( Address addr )
    : m_addr( addr ), m_memoized_valid( false )
  {}

  //-----------------------------------------------------------
  // rvalue use
  //-----------------------------------------------------------
  template<typename T>
  MemProxy<T>::operator T() const {
    DB_PRINT (("MemProxy: reading from 0x%u\n", m_addr.to_uint()));
    if ( !m_memoized_valid ) {
      //m_memoized_valid = true;
      mem::InMemStream is(m_addr);
      is >> m_memoized_value;
    }
    return m_memoized_value;
  }

  //-----------------------------------------------------------
  // lvalue uses
  //-----------------------------------------------------------
  template<typename T>
  MemProxy<T>& MemProxy<T>::operator=( T value ) {
    DB_PRINT (("MemProxy: writing %u to 0x%u\n", value, m_addr.to_uint()));
    m_memoized_value = value;
    //m_memoized_valid = true;
    mem::OutMemStream os(m_addr);
    os << m_memoized_value;
    return *this;
  }
    
  template<typename T>
  MemProxy<T>& MemProxy<T>::operator=( const MemProxy<T>& x ) {
    return operator=( static_cast<T>( x ) );
  }

//=================================================================
//=================================================================
// MemPointer
//=================================================================
//=================================================================

  //-----------------------------------------------------------
  // Constructors
  //-----------------------------------------------------------
  template<typename T>              // default
  MemPointer<T>::MemPointer()
    : m_addr( 0 ), m_obj_temp( 0 )
  {}

  template<typename T>              // from address
  MemPointer<T>::MemPointer( Address base_ptr )
    : m_addr( base_ptr ), m_obj_temp( base_ptr )
  {}

  template<typename T>              // copy
  MemPointer<T>::MemPointer( const MemPointer& p )
    : m_addr( p.m_addr ), m_obj_temp( p.m_addr )
  {}
  
  //-----------------------------------------------------------
  // * operator
  //-----------------------------------------------------------
  template<typename T>
  MemProxy<T> MemPointer<T>::operator*() const {
    return MemProxy<T>( m_addr );
  }

  //-----------------------------------------------------------
  // -> operator
  //-----------------------------------------------------------
  template<typename T>
  MemProxy<T>* MemPointer<T>::operator->() {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    m_obj_temp = MemProxy<T>( m_addr );
    return &m_obj_temp;
  }

  template<typename T>
  const MemProxy<T>* MemPointer<T>::operator->() const {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    return &m_obj_temp;
  }

  //-----------------------------------------------------------
  // = operator
  //-----------------------------------------------------------
  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const Address addr ) {
    m_addr = addr;
    m_obj_temp = MemProxy<T>( addr );
    return *this;
  }

  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const MemPointer<T>& x ) {
    m_addr = x.m_addr;
    m_obj_temp = MemProxy<T>( x.m_addr );
    return *this;
  }
  
  //-----------------------------------------------------------
  // Stream Support for reading/writing MemPointer
  //-----------------------------------------------------------
  template<typename T>
  mem::OutMemStream&
  operator<<( mem::OutMemStream& os, const MemPointer<T>& rhs ) {
    os << rhs.m_addr;
    return os;
  }

  template<typename T>
  mem::InMemStream&
  operator>>( mem::InMemStream& is, MemPointer<T>& rhs ) {
    is >> rhs.m_addr;
    return is;
  }

//=================================================================
//=================================================================
// MemPointer
//=================================================================
//=================================================================

  //-----------------------------------------------------------
  // Constructors
  //-----------------------------------------------------------
  template<typename T>
  MemProxy< MemPointer<T> >::MemProxy( Address base_ptr )
    : m_addr( base_ptr )
  {}
  
  //-----------------------------------------------------------
  // rvalue use
  //-----------------------------------------------------------
  template<typename T>
  MemProxy< MemPointer<T> >::operator MemPointer<T>() const {
    DB_PRINT (("MemProxy: reading from %u\n", m_addr));
    MemPointer<T> ptr;
    mem::InMemStream is(m_addr);
    is >> ptr;
    return ptr;
  }
  
  //-----------------------------------------------------------
  // lvalue uses
  //-----------------------------------------------------------
  template<typename T>
  MemProxy< MemPointer<T> >&
  MemProxy< MemPointer<T> >::operator=( const MemPointer<T>& p ) {
    DB_PRINT (("MemProxy: writing %u to loc %u\n", p.get_addr(), m_addr));
    mem::OutMemStream os(m_addr);
    os << p;
    return *this;
  }
  
  template<typename T>
  MemProxy< MemPointer<T> >&
  MemProxy< MemPointer<T> >::operator=( const MemProxy& x ) {
    return operator=( static_cast< MemPointer<T> >( x ) );
  }

  template<typename T>
  MemProxy< MemPointer<T> >&
  MemProxy< MemPointer<T> >::operator=( const Address x ) {
    return operator=( static_cast< MemPointer<T> >( x ) );
  }
  
  //-----------------------------------------------------------
  // * operator
  //-----------------------------------------------------------
  template<typename T>
  MemProxy< T > MemProxy< MemPointer<T> >::operator*() const {
    return *(operator MemPointer<T>());
  }
      
  //-----------------------------------------------------------
  // -> operator
  //-----------------------------------------------------------
  template<typename T>
  MemPointer<T> MemProxy< MemPointer<T> >::operator->() {
    return operator MemPointer<T>();
  }

  template<typename T>
  const MemPointer<T> MemProxy< MemPointer<T> >::operator->() const {
    return operator MemPointer<T>();
  }
      
}; // end namespace mem
