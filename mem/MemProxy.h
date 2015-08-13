//========================================================================
// MemProxy
//========================================================================
// Templated class representing a value in a test memory or real hardware
// memory. Reading this class results in reading the value from memory.
// Writing this class results in writing the value to memory.
//
// The MemProxy and MemPointer go hand-in-hand. If you take the address
// of a MemProxy then you get a MemPointer. If you dereference a
// MemPointer then you get back a MemProxy. This class is not done yet --
// it still needs work, but it is a good start.
//

#ifndef MEM_PROXY_H
#define MEM_PROXY_H

#include "MemStream.h"

namespace mem {

  // Forward Declarations

  template < typename T >
  class MemPointer;

  //======================================================================
  // MemProxy<T>
  //======================================================================
  // Represents an object stored in the explicit hardware memory.

  template < typename T >
  class MemProxy {

   public:

    //--------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------
    // Takes the address of where the object is stored in the explicit
    // hardware memory.

    explicit MemProxy( unsigned int addr );

    //--------------------------------------------------------------------
    // Cast Operator
    //--------------------------------------------------------------------
    // Enables automatically converting the MemProxy<T> into an object of
    // type T.

    operator T() const;

    //--------------------------------------------------------------------
    // Assignment Operators
    //--------------------------------------------------------------------
    // Enables assigning to a proxy with either an object of type T or
    // another MemProxy object.

    MemProxy<T>& operator=( const T& value );
    MemProxy<T>& operator=( const MemProxy<T>& rhs );

    //--------------------------------------------------------------------
    // Address Operators
    //--------------------------------------------------------------------
    // Taking the address of a proxy should return a MemPointer.

    MemPointer<T> operator&();
    const MemPointer<T> operator&() const;

   protected:

    int m_addr;

    // Memoized data members must be marked mutable since they will be
    // modified in const methods.

    mutable bool m_memoized_valid;
    mutable T    m_memoized_value;

  };

  //======================================================================
  // MemProxy< MemPointer<U> >
  //======================================================================
  // Specialization of MemProxy for MemPointers. So we essentially use
  // this when we want to represent a MemPointer stored in the explicit
  // hardware memory. Additionally provides the dereference operator. We
  // probably need to also eventually add the arrow operator.

  template < typename U >
  class MemProxy< MemPointer<U> > {

   public:

    //--------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------
    // Takes the address of where the pointer is stored in the explicit
    // hardware memory.

    explicit MemProxy( unsigned int addr );

    //--------------------------------------------------------------------
    // Cast Operator
    //--------------------------------------------------------------------
    // Enables automatically converting the MemProxy<T> into a MemPointer.

    operator MemPointer<U>() const;

    //--------------------------------------------------------------------
    // Assignment Operators
    //--------------------------------------------------------------------
    // Enables assigning to a proxy with either an int another MemProxy
    // object. NOTE: The assignment from an int is really only to support
    // assinging zero to a MemPointer. If you want to change what a
    // pointer points to then you should simply assign it the address of
    // a MemProxy.

    MemProxy< MemPointer<U> >& operator=( unsigned int value );
    MemProxy< MemPointer<U> >& operator=( const MemPointer<U>& value );
    MemProxy< MemPointer<U> >& operator=( const MemProxy< MemPointer<U> >& rhs );

    //--------------------------------------------------------------------
    // Dereference Operator
    //--------------------------------------------------------------------
    // Dereferencing a MemProxy<MemPointer<U>> returns a MemProxy just
    // like dereferencing a MemPointer<T> directly. A
    // MemProxy<MemPointer<U>> should behave like a MemPointer<T> as much
    // as possible.

    MemProxy<U> operator*();

   private:

    int m_addr;

  };

  //======================================================================
  // MemPointer<T>
  //======================================================================
  // Represents a pointer to an object in the explicit hardware memory.
  // We probably need to also eventually add the arrow operator.

  template < typename T >
  class MemPointer {

   public:

    //--------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------
    // Default constructor creates a null pointer.

    MemPointer();
    explicit MemPointer( unsigned int addr );

    //--------------------------------------------------------------------
    // Assignment Operators
    //--------------------------------------------------------------------
    // Enables assigning to a MemPointer with either an int another
    // MemProxy object. NOTE: The assignment from an int is really only
    // to support assinging zero to a MemPointer. If you want to change
    // what a pointer points to then you should simply assign it the
    // address of a MemProxy.

    MemPointer<T>& operator=( unsigned int value );
    MemPointer<T>& operator=( const MemPointer<T>& rhs );

    //--------------------------------------------------------------------
    // Comparison Operators
    //--------------------------------------------------------------------
    // Note that the overloads for int are really just for comparing to
    // zero. Usually we just want to compare one MemPointer to another
    // MemPointer.

    bool operator==( int addr ) const;
    bool operator==( const MemPointer<T>& rhs ) const;
    bool operator!=( int addr ) const;
    bool operator!=( const MemPointer<T>& rhs ) const;

    //--------------------------------------------------------------------
    // Derference Operator
    //--------------------------------------------------------------------
    // Dereferencing a MemPointer<U> returns a MemProxy<U>.

    MemProxy<T> operator*();

   private:

    template < typename U >
    friend OutMemStream&
    operator<<( OutMemStream& os, const MemPointer<U>& rhs );

    template < typename U >
    friend InMemStream&
    operator>>( InMemStream& is, MemPointer<U>& rhs );

    unsigned int m_addr;
    // T m_temp_value;

  };

  //----------------------------------------------------------------------
  // Insertion/extraction operator overloading
  //----------------------------------------------------------------------

  template < typename T >
  OutMemStream& operator<<( OutMemStream& os, const MemPointer<T>& rhs );

  template < typename T >
  InMemStream& operator>>( InMemStream& is, MemPointer<T>& rhs );

}

#include "MemProxy.inl"
#endif

