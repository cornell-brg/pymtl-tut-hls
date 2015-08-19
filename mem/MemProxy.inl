//==============================================================
// generic proxies inlines
//==============================================================

namespace mem {

//=================================================================
// MemValue
//=================================================================

  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  template<typename T>
  MemValue<T>::MemValue( Address addr )
    : m_addr( addr ), m_memoized_valid( false )
  {}

  //-----------------------------------------------------------
  // rvalue use
  //-----------------------------------------------------------
  template<typename T>
  MemValue<T>::operator T() const {
    DB_PRINT (("MemValue: reading from 0x%u\n", m_addr.to_uint()));
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
  MemValue<T>& MemValue<T>::operator=( T value ) {
    DB_PRINT (("MemValue: writing %u to 0x%u\n", value, m_addr.to_uint()));
    m_memoized_value = value;
    //m_memoized_valid = true;
    mem::OutMemStream os(m_addr);
    os << m_memoized_value;
    return *this;
  }

  template<typename T>
  MemValue<T>& MemValue<T>::operator=( const MemValue<T>& x ) {
    return operator=( static_cast<T>( x ) );
  }

//=================================================================
// MemPointer
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
  MemValue<T> MemPointer<T>::operator*() const {
    return MemValue<T>( m_addr );
  }

  //-----------------------------------------------------------
  // -> operator
  //-----------------------------------------------------------
  template<typename T>
  MemValue<T>* MemPointer<T>::operator->() {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    m_obj_temp.set_addr( m_addr );
    return &m_obj_temp;
  }

  template<typename T>
  const MemValue<T>* MemPointer<T>::operator->() const {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    return &m_obj_temp;
  }

  //-----------------------------------------------------------
  // = operator
  //-----------------------------------------------------------
  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const Address addr ) {
    m_addr = addr;
    m_obj_temp.set_addr( addr );
    return *this;
  }

  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const MemPointer<T>& x ) {
    m_addr = x.m_addr;
    m_obj_temp.set_addr( x.m_addr );
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
// MemValue of MemPointer specialization
//=================================================================

  //-----------------------------------------------------------
  // Constructors
  //-----------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >::MemValue( Address base_ptr )
    : m_addr( base_ptr )
  {}

  //-----------------------------------------------------------
  // rvalue use
  //-----------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >::operator MemPointer<T>() const {
    DB_PRINT (("MemValue: reading from %u\n", m_addr));
    MemPointer<T> ptr;
    mem::InMemStream is(m_addr);
    is >> ptr;
    return ptr;
  }

  //-----------------------------------------------------------
  // lvalue uses
  //-----------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >&
  MemValue< MemPointer<T> >::operator=( const MemPointer<T>& p ) {
    DB_PRINT (("MemValue: writing %u to loc %u\n", p.get_addr(), m_addr));
    mem::OutMemStream os(m_addr);
    os << p;
    return *this;
  }

  template<typename T>
  MemValue< MemPointer<T> >&
  MemValue< MemPointer<T> >::operator=( const MemValue& x ) {
    return operator=( static_cast< MemPointer<T> >( x ) );
  }

  template<typename T>
  MemValue< MemPointer<T> >&
  MemValue< MemPointer<T> >::operator=( const Address x ) {
    return operator=( static_cast< MemPointer<T> >( x ) );
  }

  //-----------------------------------------------------------
  // * operator
  //-----------------------------------------------------------
  template<typename T>
  MemValue< T > MemValue< MemPointer<T> >::operator*() const {
    return *(operator MemPointer<T>());
  }

  //-----------------------------------------------------------
  // -> operator
  //-----------------------------------------------------------
  template<typename T>
  MemPointer<T> MemValue< MemPointer<T> >::operator->() {
    return operator MemPointer<T>();
  }

  template<typename T>
  const MemPointer<T> MemValue< MemPointer<T> >::operator->() const {
    return operator MemPointer<T>();
  }

}; // end namespace mem
