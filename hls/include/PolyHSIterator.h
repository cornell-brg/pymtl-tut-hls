#ifndef POLYHS_ITERATOR_H
#define POLYHS_ITERATOR_H

#include "PolyHSValue.h"
#include "PolyHSReferenceProxy.h"

//-------------------------------------------------------------------
// PolyHSIterator
//-------------------------------------------------------------------
class PolyHSIterator {

  public:
    //typedef std::random_accessPolyHSIterator_tag   iterator_category;
    //typedef ptrdiff_t                         difference_type;
    typedef PolyHSValue                       value_type;
    typedef PolyHSIterator                    pointer;
    typedef PolyHSReferenceProxy              reference;
    
  private:
    // data structure id
    unsigned m_ds_id;

    // index to an data element
    unsigned m_index;
    
    // metadata
    const ap_uint<8> m_type;
    const ap_uint<8> m_fields;

  public:

    //--------------------------------------------------------------
    //  constructors
    //--------------------------------------------------------------

    // default constructor
    PolyHSIterator() : m_ds_id( 0 ), m_index( 0 ), m_type( 0 ) {}

    PolyHSIterator( unsigned ds_id, unsigned index, 
               const ap_uint<8> type, const ap_uint<8> fields ) :
      m_ds_id( ds_id ), m_index( index ), m_type( type ), m_fields( fields ) {}

    // copy constructor
    PolyHSIterator( const PolyHSIterator& it )
      : m_ds_id( it.m_ds_id ), m_index( it.m_index ), 
        m_type( it.m_type ), m_fields( it.m_fields) {}

    //--------------------------------------------------------------
    // set_state
    //--------------------------------------------------------------

    // set function for ds_id
    void set_state( unsigned ds_id, unsigned index )
    {
      m_ds_id = ds_id;
      m_index = index;
    }
    
    //--------------------------------------------------------------
    // get functions
    // RZ: This is needed because the ASU does cannot return an
    // iterator, it must return a 32bit value. At the very least
    // we need access to m_index so it can be returned by the ASU
    //--------------------------------------------------------------
    
    unsigned get_index() const
    {
      return m_index;
    }

    //--------------------------------------------------------------
    // overloaded operators
    //--------------------------------------------------------------

    // assignment
    PolyHSIterator& operator=( const PolyHSIterator& rhs )
    {
      m_ds_id = rhs.m_ds_id;
      m_index = rhs.m_index;
      return *this;
    }

    // prefix increment
    PolyHSIterator& operator++()
    {
      ++m_index;
      return *this;
    }

    // postfix increment
    PolyHSIterator operator++( int )
    {
      PolyHSIterator tmp( *this );
      operator++();
      return tmp;
    }

    // prefix decrement
    PolyHSIterator& operator--()
    {
      --m_index;
      return *this;
    }

    // postfix decrement
    PolyHSIterator operator--( int )
    {
      PolyHSIterator tmp( *this );
      operator--();
      return tmp;
    }

    // equality compare
    bool operator==( const PolyHSIterator& rhs ) const
    {
      return ( m_index == rhs.m_index );
    }

    // not equal compare
    bool operator!=( const PolyHSIterator& rhs ) const
    {
      return !( *this == rhs );
    }

    // less than compare
    bool operator<( const PolyHSIterator& rhs ) const
    {
      return ( m_index < rhs.m_index );
    }

    // less than equal compare
    bool operator<=( const PolyHSIterator& rhs ) const
    {
      return ( m_index <= rhs.m_index );
    }

    // greater than compare
    bool operator>( const PolyHSIterator& rhs ) const
    {
      return !( *this <= rhs );
    }

    // greater than equal compare
    bool operator>=( const PolyHSIterator& rhs ) const
    {
      return !( *this < rhs );
    }

    // random access operators
    PolyHSIterator& operator+=( unsigned i )
    {
      m_index += i;
      return *this;
    }

    PolyHSIterator operator+( unsigned i ) const
    {
      PolyHSIterator tmp( *this );
      tmp += i;
      return tmp;
    }

    PolyHSIterator& operator-=( unsigned i )
    {
      m_index -= i;
      return *this;
    }

    PolyHSIterator operator-( unsigned i ) const
    {
      PolyHSIterator tmp( *this );
      tmp -= i;
      return tmp;
    }

    // dereference operator will return a PolyHSReferenceProxy object
    reference operator*() const
    {
      return reference( m_ds_id, m_index, m_type, m_fields );
    }

}; // end class PolyHSIterator

#endif
