//==============================================================
// generic proxies header
//==============================================================

#ifndef PROXY_H
#define PROXY_H

#include <stdlib.h>
#include <stdio.h>
#ifndef CPP_COMPILE
  #include <ap_utils.h>
#endif

#ifdef DEBUG
  #define DB_PRINT(x) printf x
  #define DB_ASSERT(x) assert x
#else
  #define DB_PRINT(x)
  #define DB_ASSERT(x)
#endif

template<typename T> class ValueProxy;
template<typename T> class PointerProxy;

//---------------------------------------------------------------
// ValueProxy
// This proxy can wrap any non-pointer object.
// For non-primitive objects you can't access fields directly
// from the ValueProxy, you have to get the object out first.
//---------------------------------------------------------------
template<typename T>
class ValueProxy {
  typedef PointerProxy< ValueProxy<T> > PtrProxy;

  Address m_addr;

  public:
    //-----------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------
    ValueProxy( Address addr ) : m_addr( addr ) {}
    ValueProxy( const ValueProxy& p ) : m_addr( p.m_addr ) {}
    static size_t size() {
      return sizeof(T) > PTR_SIZE ? sizeof(T) : PTR_SIZE;
    }

    //-----------------------------------------------------------
    // & operator
    //-----------------------------------------------------------
    PtrProxy operator&() {
      return PtrProxy( m_addr );
    }
    const PtrProxy operator&() const {
      return PtrProxy( m_addr );
    }

    //-----------------------------------------------------------
    // rvalue and lvalue uses
    //-----------------------------------------------------------
    operator T() const {
      DB_PRINT(("ValueProxy read: m_addr=%llu\n", (long long unsigned)m_addr));
      #ifdef CPP_COMPILE
        return *((T*)m_addr);
      #else
        memreq.write( MemReqMsg( 0, sizeof(T), m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        //XXX: NOOOOOOOO
        return (T)mem_resp.data;
      #endif
    }
    ValueProxy& operator= ( T data ) {
      DB_PRINT(("ValueProxy write: m_addr=%llu\n", (long long unsigned)m_addr));
      #ifdef CPP_COMPILE
        *((T*)m_addr) = data;
      #else
        memreq.write( MemReqMsg( data, sizeof(T), m_addr, 0, MSG_WRITE ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
      #endif
      return *this;
    }
    ValueProxy& operator=( const ValueProxy& x ) {
      return operator=( static_cast<T>( x ) );
    }

    //-----------------------------------------------------------
    // other
    //-----------------------------------------------------------
    Address get_addr() const { return m_addr; }
    void set_addr( const Address addr ) { m_addr = addr; }
};

template<typename T>
inline bool operator==( const ValueProxy<T>& lhs,
                       const T& rhs ) {
  return (T)lhs == rhs;
}
template<typename T>
inline bool operator!=( const ValueProxy<T>& lhs,
                       const T& rhs ) {
  return (T)lhs != rhs;
}
template<typename T>
inline bool operator<( const ValueProxy<T>& lhs,
                       const T& rhs ) {
  return (T)lhs < rhs;
}

//---------------------------------------------------------------
// PointerProxy
// This proxy wraps pointer objects, it supports the * and ->
// operators as pointer-like objects do.
//---------------------------------------------------------------
template<typename T>
class PointerProxy {
  Address m_addr;
  T m_obj_temp;

  public:
    //-----------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------
    PointerProxy()
      : m_addr( 0 ), m_obj_temp( 0 )
    {}
#ifndef CPP_COMPILE
    PointerProxy( unsigned base_ptr )
      : m_addr( base_ptr ), m_obj_temp( base_ptr )
    {}
#endif
    PointerProxy( Address base_ptr )
      : m_addr( base_ptr ), m_obj_temp( base_ptr )
    {}
    PointerProxy( const PointerProxy& p )
      : m_addr( p.m_addr ), m_obj_temp( p.m_obj_temp.get_addr() )
    {}
    static size_t size() {
      return PTR_SIZE;
    }
    
    //-----------------------------------------------------------
    // * and -> operators
    //-----------------------------------------------------------
    T operator* () const {
      return T( m_addr );
    }
    T* operator-> () {
      DB_ASSERT(( m_addr != 0 ));
      set_addr( m_obj_temp, m_addr );
      return &m_obj_temp;
    }
    const T* operator-> () const {
      DB_ASSERT(( m_addr != 0 ));
      return &m_obj_temp;
    }
    
    //-----------------------------------------------------------
    // = operator
    //-----------------------------------------------------------
    PointerProxy& operator=( const Address addr ) {
      m_addr = addr;
      set_addr( m_obj_temp, addr );
      return *this;
    }
    PointerProxy& operator=( const PointerProxy& x ) {
      m_addr = x.m_addr;
      set_addr( m_obj_temp, x.m_addr );
      return *this;
    }

    //-----------------------------------------------------------
    // Pointer conversion (for pointer comp)
    //-----------------------------------------------------------
    operator Address() const { return m_addr; }

    //-----------------------------------------------------------
    // Other
    //-----------------------------------------------------------
    Address get_addr() const { return m_addr; }
};

template<typename T>
inline bool operator==( const PointerProxy<T>& lhs,
                        const PointerProxy<T>& rhs ) {
  return lhs.get_addr() == rhs.get_addr();
}
template<typename T>
inline bool operator!=( const PointerProxy<T>& lhs,
                        const PointerProxy<T>& rhs ) {
  return lhs.get_addr() != rhs.get_addr();
}

template<typename T>
inline bool operator&&( const PointerProxy<T>& lhs,
                        const bool rhs ) {
  return (Address)lhs && rhs;
}
template<typename T>
inline bool operator||( const PointerProxy<T>& lhs,
                        const bool rhs ) {
  return (Address)lhs || rhs;
}

#ifndef CPP_COMPILE
template<typename T>
inline bool operator==( const PointerProxy<T>& lhs,
                        const unsigned rhs ) {
  return lhs.get_addr() == rhs;
}
template<typename T>
inline bool operator!=( const PointerProxy<T>& lhs,
                        const unsigned rhs ) {
  return lhs.get_addr() != rhs;
}
#endif


#endif
