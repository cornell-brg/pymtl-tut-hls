//==============================================================
// generic proxies header
//==============================================================

#ifndef PROXY_H
#define PROXY_H

#include <stdio.h>
#include <ap_utils.h>
#include "MemStream.h"

#ifdef DEBUG
  #define DB_PRINT(x) printf x
  #define DB_ASSERT(x) assert x
#else
  #define DB_PRINT(x)
  #define DB_ASSERT(x)
#endif

namespace mem {

  //XXX:This is only true for HLS or HLS_TESTING.
  //    For C++ compilation, PTR_SIZE should be sizeof(void*)
  //    which is 8 on brg
  static unsigned PTR_SIZE = 4;

  typedef unsigned        Address;

  template<typename T> class MemProxy;
  template<typename T> class MemPointer;

  //========================================================================
  //========================================================================
  // MemProxy
  // This proxy can wrap any non-pointer object.
  // For non-primitive objects you can't access fields directly
  // from the MemProxy, you have to get the object out first.
  //========================================================================
  //========================================================================
  template<typename T>
  class MemProxy {
    Address m_addr;

    mutable bool m_memoized_valid;
    mutable T    m_memoized_value;

    public:
      //-----------------------------------------------------------
      // Constructor
      //-----------------------------------------------------------
      explicit MemProxy( Address addr );

      //-----------------------------------------------------------
      // rvalue and lvalue uses
      //-----------------------------------------------------------
      operator T() const;
      MemProxy<T>& operator=( T data );
      MemProxy<T>& operator=( const MemProxy<T>& x );
      
      //-----------------------------------------------------------
      // & operator
      //-----------------------------------------------------------
      MemPointer<T> operator&() { return MemPointer<T>( m_addr ); }
      const MemPointer<T> operator&() const { return MemPointer<T>( m_addr ); }
      
      //-----------------------------------------------------------
      // Comparison Operators
      //-----------------------------------------------------------
      bool operator==( const T& rhs ) const { return operator T() == rhs; }
      bool operator!=( const T& rhs ) const { return operator T() != rhs; }
      bool operator< ( const T& rhs ) const { return operator T() <  rhs; }

      //-----------------------------------------------------------
      // other
      //-----------------------------------------------------------
      Address get_addr() const { return m_addr; }
      void set_addr( const Address addr ) { m_addr = addr; }
      static size_t size() {
        return sizeof(T) > PTR_SIZE ? sizeof(T) : PTR_SIZE;
      }
  };

  //========================================================================
  //========================================================================
  // MemPointer
  // This proxy wraps pointer objects, it supports the * and ->
  // operators as pointer-like objects do.
  //========================================================================
  //========================================================================
  template<typename T>
  class MemPointer {

    Address m_addr;
    MemProxy<T> m_obj_temp;

    public:
      //-----------------------------------------------------------
      // Constructors
      //-----------------------------------------------------------
      MemPointer();
      explicit MemPointer( Address base_ptr );
      MemPointer( const MemPointer& p );
      
      //-----------------------------------------------------------
      // * and -> operators
      //-----------------------------------------------------------
      MemProxy<T> operator*() const;
      MemProxy<T>* operator->();
      const MemProxy<T>* operator-> () const;
      
      //-----------------------------------------------------------
      // = operator
      //-----------------------------------------------------------
      MemPointer<T>& operator=( const Address addr );
      MemPointer<T>& operator=( const MemPointer<T>& x );
      
      //-----------------------------------------------------------
      // Comparison Operators
      //-----------------------------------------------------------
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
      
      //-----------------------------------------------------------
      // Streams
      //-----------------------------------------------------------
      template<typename U>
      friend OutMemStream&
      operator<<( OutMemStream& os, const MemPointer<U>& rhs );
      
      template<typename U>
      friend InMemStream&
      operator>>( InMemStream& is, MemPointer<U>& rhs );

      //-----------------------------------------------------------
      // other
      //-----------------------------------------------------------
      Address get_addr() const { return m_addr; }
      static size_t size() { return PTR_SIZE; }
  };

  template<typename T>
  OutMemStream&
  operator<<( OutMemStream& os, const MemPointer<T>& rhs );

  template<typename T>
  InMemStream&
  operator>>( InMemStream& is, MemPointer<T>& rhs );

  //========================================================================
  //========================================================================
  // MemProxy of MemPointer specialization
  //========================================================================
  //========================================================================
  template<typename T>
  class MemProxy< MemPointer<T> > {
      Address m_addr;

    public:
      //----------------------------------------------------------------
      // Constructors
      //----------------------------------------------------------------
      explicit MemProxy( Address base_ptr );

      //----------------------------------------------------------------
      // rvalue and lvalue uses
      //----------------------------------------------------------------
      operator MemPointer<T>() const;
      MemProxy& operator=( const MemPointer<T>& p );
      MemProxy& operator=( const MemProxy& x );
      MemProxy& operator=( const Address x );
      
      //----------------------------------------------------------------
      // * and -> operators
      //----------------------------------------------------------------
      MemProxy<T> operator*() const;
      MemPointer<T> operator->();
      const MemPointer<T> operator->() const;

      //----------------------------------------------------------------
      // Comparison Operators
      //----------------------------------------------------------------
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

      //----------------------------------------------------------------
      // other
      //----------------------------------------------------------------
      Address get_addr() const { return m_addr; }
      void set_addr( const Address addr ) { m_addr = addr; }
  };

}; // end namespace mem

#include "MemProxy.inl"

#endif
