//========================================================================
// generic proxies header
//========================================================================

#ifndef MEM_MEM_PROXY_H
#define MEM_MEM_PROXY_H

#include <stdio.h>
#include <ap_utils.h>
#include "mem/MemStream.h"

#ifdef DEBUG
  #define DB_PRINT(x) printf x
  #define DB_ASSERT(x) assert x
#else
  #define DB_PRINT(x)
  #define DB_ASSERT(x)
#endif

namespace mem {

  template<typename T> class MemValue;
  template<typename T> class MemPointer;

  //======================================================================
  // MemValue
  //======================================================================
  // This proxy can wrap any non-pointer object.
  // For non-primitive objects you can't access fields directly
  // from the MemValue, you have to get the object out first.
  template<typename T>
  class MemValue {

      Address m_addr;

      mutable bool m_memoized_valid;
      mutable T    m_memoized_value;

      MemReqStream&  m_memreq;
      MemRespStream& m_memresp;

    public:
      //------------------------------------------------------------------
      // Constructor
      //------------------------------------------------------------------
      explicit MemValue( Address addr, MemReqStream& memreq, MemRespStream& memresp );

      //------------------------------------------------------------------
      // rvalue and lvalue uses
      //------------------------------------------------------------------
      operator T() const;
      MemValue<T>& operator=( T data );
      MemValue<T>& operator=( const MemValue<T>& x );

      //------------------------------------------------------------------
      // & operator
      //------------------------------------------------------------------
      MemPointer<T> operator&() { return MemPointer<T>( m_addr, m_memreq, m_memresp ); }
      const MemPointer<T> operator&() const { return MemPointer<T>( m_addr, m_memreq, m_memresp ); }

      //------------------------------------------------------------------
      // Comparison Operators
      //------------------------------------------------------------------
      bool operator==( const T& rhs ) const { return operator T() == rhs; }
      bool operator!=( const T& rhs ) const { return operator T() != rhs; }
      bool operator< ( const T& rhs ) const { return operator T() <  rhs; }

      //------------------------------------------------------------------
      // Get/Set Address
      //------------------------------------------------------------------
      Address get_addr() const { return m_addr; }
      void set_addr( const Address addr ) { m_addr = addr; }
      static size_t size() {
        return sizeof(T) > PTR_SIZE ? sizeof(T) : PTR_SIZE;
      }
      
      MemReqStream& memreq()  const { return m_memreq; }
      MemReqStream& memresp() const { return m_memresp; }
  };

  //======================================================================
  // MemPointer
  //======================================================================
  // This proxy wraps pointer objects, it supports the * and ->
  // operators as pointer-like objects do.
  template<typename T>
  class MemPointer {

      Address m_addr;

      mutable MemValue<T> m_obj_temp;

    public:
      //------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------
      MemPointer( MemReqStream& memreq, MemRespStream& memresp );
      explicit MemPointer( Address base_ptr, MemReqStream& memreq, MemRespStream& memresp );
      MemPointer( const MemPointer& p );

      //------------------------------------------------------------------
      // * and -> operators
      //------------------------------------------------------------------
      MemValue<T> operator*() const;
      MemValue<T>* operator->();
      const MemValue<T>* operator->() const;

      //------------------------------------------------------------------
      // = operator
      //------------------------------------------------------------------
      MemPointer<T>& operator=( const Address addr );
      MemPointer<T>& operator=( const MemPointer<T>& x );

      //------------------------------------------------------------------
      // Unary Operators
      //------------------------------------------------------------------
      bool operator!() const {
        return m_addr == 0;
      }

      //------------------------------------------------------------------
      // Comparison Operators
      //------------------------------------------------------------------
      bool operator==( const MemPointer& rhs ) const {
        return m_addr == rhs.m_addr;
      }
      bool operator!=( const MemPointer& rhs ) const {
        return m_addr != rhs.m_addr;
      }
      bool operator==( const Address rhs ) const {
        return m_addr == rhs;
      }
      bool operator!=( const Address rhs ) const {
        return m_addr != rhs;
      }

      //------------------------------------------------------------------
      // Streams
      //------------------------------------------------------------------
      template<typename U>
      friend OutMemStream&
      operator<<( OutMemStream& os, const MemPointer<U>& rhs );

      template<typename U>
      friend InMemStream&
      operator>>( InMemStream& is, MemPointer<U>& rhs );

      //------------------------------------------------------------------
      // Get/Set Address
      //------------------------------------------------------------------
      Address get_addr() const { return m_addr; }
      static size_t size() { return PTR_SIZE; }
  };

  template<typename T>
  OutMemStream&
  operator<<( OutMemStream& os, const MemPointer<T>& rhs );

  template<typename T>
  InMemStream&
  operator>>( InMemStream& is, MemPointer<T>& rhs );

  //======================================================================
  // MemValue of MemPointer specialization
  //======================================================================
  template<typename T>
  class MemValue< MemPointer<T> > {

      Address m_addr;
    
      MemReqStream&  m_memreq;
      MemRespStream& m_memresp;

    public:
      //------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------
      explicit MemValue( Address addr, MemReqStream& memreq, MemRespStream& memresp );

      //------------------------------------------------------------------
      // rvalue and lvalue uses
      //------------------------------------------------------------------
      operator MemPointer<T>() const;
      MemValue& operator=( const MemPointer<T>& p );
      MemValue& operator=( const MemValue& x );
      MemValue& operator=( const Address x );

      //------------------------------------------------------------------
      // * and -> operators
      //------------------------------------------------------------------
      MemValue<T> operator*() const;
      MemPointer<T> operator->();
      const MemPointer<T> operator->() const;

      //------------------------------------------------------------------
      // Unary Operators
      //------------------------------------------------------------------
      bool operator!() const {
        return (operator MemPointer<T>() == 0);
      }

      //------------------------------------------------------------------
      // Comparison Operators
      //------------------------------------------------------------------
      bool operator==( const MemPointer<T>& rhs ) const {
        return operator MemPointer<T>() == rhs;
      }
      bool operator!=( const MemPointer<T>& rhs ) const {
        return operator MemPointer<T>() != rhs;
      }
      bool operator==( const Address rhs ) const {
        return operator MemPointer<T>() == rhs;
      }
      bool operator!=( const Address rhs ) const {
        return operator MemPointer<T>() != rhs;
      }

      //------------------------------------------------------------------
      // Get/Set Address
      //------------------------------------------------------------------
      Address get_addr() const { return m_addr; }
      void set_addr( const Address addr ) { m_addr = addr; }

      MemReqStream&  memreq()  const { return m_memreq; }
      MemRespStream& memresp() const { return m_memresp; }
  };

}; // end namespace mem

#include "mem/MemProxy.inl"
#endif /* MEM_MEM_PROXY_H */

