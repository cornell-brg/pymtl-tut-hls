#ifndef ITERATOR_H
#define ITERATOR_H

#include "TypeTraits.h"

namespace polyhs {
  //------------------------------------------------------------------
  // _iterator Class
  //------------------------------------------------------------------

  template<typename T>
  class _iterator {
    public:
      // associated types used for the iterator
      //typedef std::random_access_iterator_tag   iterator_category;
      typedef TypeTraits<T>                     Traits;
      typedef T                                 value_type;
      typedef _iterator                         pointer;
      typedef typename Traits::reference        reference;

    private:
      //--------------------------------------------------------------
      // Hardware iterator abstraction
      //--------------------------------------------------------------
      // Data members that serve as the state used by hardware models
      // to abstract a location in the sequence.
      //
      // NOTE: If the hardware iterator abstraction changes then we
      // need to modify these members and provide setters/getters if
      // required.
      //
      //  8 bits  24 bits
      // +------+-----------+
      // | id   | iter_bits |
      // +------+-----------+

      // hardware iterator
      unsigned m_iter;

    public:
      //--------------------------------------------------------------
      //  Constructors
      //--------------------------------------------------------------

      // default constructor
      _iterator() : m_iter( 0 ) {}

      _iterator( unsigned ds_id, int index ) {
        SET_ID  ( m_iter, ds_id );
        SET_ITER( m_iter, index );
      }

      // copy constructor
      _iterator( const _iterator& it ) : m_iter( it.m_iter ){}

      //--------------------------------------------------------------
      // Hardware iterator abstraction
      //--------------------------------------------------------------
      // Set function to initialize an iterator explicitly

      // set function for m_ds_id, m_index
      void set_state( unsigned ds_id, int index )
      {
        SET_ID  ( m_iter, ds_id );
        SET_ITER( m_iter, index );
      }

      //--------------------------------------------------------------
      // Overloaded operators
      //--------------------------------------------------------------

      // assignment
      _iterator& operator=( const _iterator& rhs )
      {
        m_iter = rhs.m_iter;
        return *this;
      }

      // prefix increment
      _iterator& operator++()
      {
        SET_ITER( m_iter, (GET_ITER( m_iter )+1) );
        return *this;
      }

      // postfix increment
      _iterator operator++( int )
      {
        _iterator tmp( *this );
        operator++();
        return tmp;
      }

      // prefix decrement
      _iterator& operator--()
      {
        SET_ITER( m_iter, (GET_ITER( m_iter )-1) );
        return *this;
      }

      // postfix decrement
      _iterator operator--( int )
      {
        _iterator tmp( *this );
        operator--();
        return tmp;
      }

      // equality compare
      bool operator==( const _iterator& rhs ) const
      {
        return ( m_iter == rhs.m_iter );
      }

      // not equal compare
      bool operator!=( const _iterator& rhs ) const
      {
        return !( *this == rhs );
      }

      // less than compare
      bool operator<( const _iterator& rhs ) const
      {
        return ( m_iter < rhs.m_iter );
      }

      // less than equal compare
      bool operator<=( const _iterator& rhs ) const
      {
        return ( m_iter <= rhs.m_iter );
      }

      // greater than compare
      bool operator>( const _iterator& rhs ) const
      {
        return !( *this <= rhs );
      }

      // greater than equal compare
      bool operator>=( const _iterator& rhs ) const
      {
        return !( *this < rhs );
      }

      // random access operators
      _iterator& operator+=( int i )
      {
        SET_ITER( m_iter, (GET_ITER( m_iter ) + i) );
        return *this;
      }

      // Arithmetic operators
      _iterator operator+( int i ) const
      {
        _iterator tmp( *this );
        tmp += i;
        return tmp;
      }

      _iterator& operator-=( int i )
      {
        SET_ITER( m_iter, (GET_ITER( m_iter ) - i) );
        return *this;
      }

      _iterator operator-( int i ) const
      {
        _iterator tmp( *this );
        tmp -= i;
        return tmp;
      }

      // shreesha:
      // not sure if these are correct...
      // maybe I should care about the ds_id as well?
      int operator+( const _iterator& rhs )
      {
        return ( GET_ITER( m_iter ) + GET_ITER( rhs.m_iter ) );
      }

      int operator-( const _iterator& rhs )
      {
        return ( GET_ITER( m_iter ) - GET_ITER( rhs.m_iter ) );
      }

      // Dereference operator will return a ReferenceProxy object
      reference operator*() const
      {
        return reference( m_iter );
      }
  };
}
#endif
