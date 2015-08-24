//========================================================================
// generic proxies inlines
//========================================================================

namespace xmem {

//========================================================================
// MemValue
//========================================================================

  //----------------------------------------------------------------------
  // Constructor
  //----------------------------------------------------------------------
  template<typename T>
  MemValue<T>::MemValue( Address addr, MemReqStream& memreq,
                         MemRespStream& memresp )
    : m_addr( addr ),
      m_memoized_valid( false ),
      m_memreq( memreq ), m_memresp( memresp )
  {}

  //----------------------------------------------------------------------
  // rvalue use
  //----------------------------------------------------------------------
  template<typename T>
  MemValue<T>::operator T() const {
    DB_PRINT (("MemValue: reading from 0x%u\n", m_addr.to_uint()));
    if ( !m_memoized_valid ) {
      //m_memoized_valid = true;
      xmem::InMemStream is(m_addr,m_memreq,m_memresp);
      is >> m_memoized_value;
    }
    return m_memoized_value;
  }

  //----------------------------------------------------------------------
  // lvalue uses
  //----------------------------------------------------------------------
  template<typename T>
  MemValue<T>& MemValue<T>::operator=( T value ) {
    DB_PRINT (("MemValue: writing %u to 0x%u\n", value, m_addr.to_uint()));
    // dump the memoized value on a store
    m_memoized_valid = false;
    xmem::OutMemStream os(m_addr,m_memreq,m_memresp);
    os << value;
    return *this;
  }

  template<typename T>
  MemValue<T>& MemValue<T>::operator=( const MemValue<T>& x ) {
    return operator=( static_cast<T>( x ) );
  }

//========================================================================
// MemPointer
//========================================================================

  //----------------------------------------------------------------------
  // Constructors
  //----------------------------------------------------------------------
  template<typename T>              // default
  MemPointer<T>::MemPointer( MemReqStream& memreq, MemRespStream& memresp )
    : m_addr( 0 ),
      m_obj_temp( 0, memreq, memresp )
  {}

  template<typename T>              // from address
  MemPointer<T>::MemPointer( Address base_ptr, MemReqStream& memreq,
                             MemRespStream& memresp )
    : m_addr( base_ptr ),
      m_obj_temp( base_ptr, memreq, memresp )
  {}

  template<typename T>              // copy
  MemPointer<T>::MemPointer( const MemPointer& p )
    : m_addr( p.m_addr ),
      m_obj_temp( p.m_addr, p.m_obj_temp.memreq(), p.m_obj_temp.memresp() )
  {}

  //----------------------------------------------------------------------
  // * operator
  //----------------------------------------------------------------------
  template<typename T>
  MemValue<T> MemPointer<T>::operator*() const {
    return MemValue<T>( m_addr, m_obj_temp.memreq(), m_obj_temp.memresp() );
  }

  //----------------------------------------------------------------------
  // -> operator
  //----------------------------------------------------------------------
  template<typename T>
  MemValue<T>* MemPointer<T>::operator->() {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    m_obj_temp.set_addr( m_addr );
    return &m_obj_temp;
  }

  template<typename T>
  const MemValue<T>* MemPointer<T>::operator->() const {
    DB_PRINT(("MemPointer: operator->: m_addr=%u\n", m_addr));
    m_obj_temp.set_addr( m_addr );
    return &m_obj_temp;
  }

  //----------------------------------------------------------------------
  // = operator
  //----------------------------------------------------------------------
  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const Address addr ) {
    m_addr = addr;
    return *this;
  }

  template<typename T>
  MemPointer<T>& MemPointer<T>::operator=( const MemPointer<T>& x ) {
    m_addr = x.m_addr;
    return *this;
  }

  //----------------------------------------------------------------------
  // Stream Support for reading/writing MemPointer
  //----------------------------------------------------------------------
  template<typename T>
  xmem::OutMemStream&
  operator<<( xmem::OutMemStream& os, const MemPointer<T>& rhs ) {
    os << rhs.m_addr;
    return os;
  }

  template<typename T>
  xmem::InMemStream&
  operator>>( xmem::InMemStream& is, MemPointer<T>& rhs ) {
    is >> rhs.m_addr;
    return is;
  }

//========================================================================
// MemValue of MemPointer specialization
//========================================================================

  //----------------------------------------------------------------------
  // Constructors
  //----------------------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >::MemValue( Address base_ptr, MemReqStream& memreq,
                                       MemRespStream& memresp )
    : m_addr( base_ptr ), m_memreq( memreq ), m_memresp( memresp )
  {}

  //----------------------------------------------------------------------
  // rvalue use
  //----------------------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >::operator MemPointer<T>() const {
    DB_PRINT (("MemValue: reading from %u\n", m_addr));
    MemPointer<T> ptr(0,m_memreq,m_memresp);
    xmem::InMemStream is(m_addr,m_memreq,m_memresp);
    is >> ptr;
    return ptr;
  }

  //----------------------------------------------------------------------
  // lvalue uses
  //----------------------------------------------------------------------
  template<typename T>
  MemValue< MemPointer<T> >&
  MemValue< MemPointer<T> >::operator=( const MemPointer<T>& p ) {
    DB_PRINT (("MemValue: writing %u to loc %u\n", p.get_addr(), m_addr));
    xmem::OutMemStream os(m_addr,m_memreq,m_memresp);
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
    return operator=( MemPointer<T>( x, m_memreq, m_memresp ) );
  }

  //----------------------------------------------------------------------
  // * operator
  //----------------------------------------------------------------------
  template<typename T>
  MemValue< T > MemValue< MemPointer<T> >::operator*() const {
    return *(operator MemPointer<T>());
  }

  //----------------------------------------------------------------------
  // -> operator
  //----------------------------------------------------------------------
  template<typename T>
  MemPointer<T> MemValue< MemPointer<T> >::operator->() {
    return operator MemPointer<T>();
  }

  template<typename T>
  const MemPointer<T> MemValue< MemPointer<T> >::operator->() const {
    return operator MemPointer<T>();
  }

}; // end namespace xmem

