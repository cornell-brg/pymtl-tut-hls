#ifndef ITERATOR_H
#define ITERATOR_H

#include "ItuIface.h"

template<typename T> class ReferenceProxy;
template<typename T> class _iterator;

extern ItuIfaceType g_itu_iface;

//-------------------------------------------------------------------
// Class ReferenceProxy
//-------------------------------------------------------------------
template<typename T>
class ReferenceProxy {
  friend class _iterator<T>;

  private:
    // data structure id
    unsigned m_ds_id;

    // index to an data element
    int      m_index;


  public:

    ReferenceProxy() : m_ds_id( 0 ), m_index( 0 ) {}
    ReferenceProxy( unsigned ds_id, int index )
      : m_ds_id( ds_id ), m_index( index ) {}
    //ReferenceProxy( unsigned ds_id, int index, volatile ItuIfaceType* iface )
    //  : m_ds_id( ds_id ), m_index( index ), m_itu_iface(iface) {}

    ~ReferenceProxy(){}

    operator T() const
    {
      //#pragma HLS INLINE self off
      //send a read request
      ItuReqType req = 0;
      req |= m_ds_id;
      req = (req << 32) | m_index;
      req = (req << 32);
      g_itu_iface.req = req;

      ItuRespType resp = g_itu_iface.resp;
      T data = resp << 1;
      return data;
    }

    ReferenceProxy& operator=( T data )
    {
      //#pragma HLS INLINE self off
      //send a write request
      ItuReqType req = 1;
      req = (req << 32) | m_ds_id;
      req = (req << 32) | m_index;
      req = (req << 32) | (data & 0xFF);
      g_itu_iface.req = req;

      ItuRespType resp = g_itu_iface.resp;
      return *this;
    }

    ReferenceProxy& operator=( const ReferenceProxy& x )
    {
      return operator=( static_cast<T>( x ) );
    }

}; // class ReferenceProxy

//-------------------------------------------------------------------
// Class _iterator
//-------------------------------------------------------------------
template<typename T>
class _iterator {

  public:
    //typedef std::random_access_iterator_tag   iterator_category;
    //typedef ptrdiff_t                         difference_type;
    typedef T                                 value_type;
    typedef _iterator                         pointer;
    typedef ReferenceProxy<T>                 reference;
    
  private:
    // data structure id
    unsigned m_ds_id;

    // index to an data element
    int      m_index;

    //
    //static ItuIfaceType m_iface;

  public:

    //--------------------------------------------------------------
    //  constructors
    //--------------------------------------------------------------

    // default constructor
    _iterator() : m_ds_id( 0 ), m_index( 0 ) {}

    _iterator( unsigned ds_id, int index) :
      m_ds_id( ds_id ), m_index( index ) {}

    // copy constructor
    _iterator( const _iterator& it )
      : m_ds_id( it.m_ds_id ), m_index( it.m_index ) {}

    //--------------------------------------------------------------
    // set_state
    //--------------------------------------------------------------

    // set function for ds_id
    void set_state( unsigned ds_id, int index )
    {
      m_ds_id = ds_id;
      m_index = index;
    }

    //--------------------------------------------------------------
    // overloaded operators
    //--------------------------------------------------------------

    // assignment
    _iterator& operator=( const _iterator& rhs )
    {
      m_ds_id = rhs.m_ds_id;
      m_index = rhs.m_index;
      return *this;
    }

    // prefix increment
    _iterator& operator++()
    {
      ++m_index;
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
      --m_index;
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
      return ( m_index == rhs.m_index );
    }

    // not equal compare
    bool operator!=( const _iterator& rhs ) const
    {
      return !( *this == rhs );
    }

    // less than compare
    bool operator<( const _iterator& rhs ) const
    {
      return ( m_index < rhs.m_index );
    }

    // less than equal compare
    bool operator<=( const _iterator& rhs ) const
    {
      return ( m_index <= rhs.m_index );
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
      m_index += i;
      return *this;
    }

    _iterator operator+( int i ) const
    {
      _iterator tmp( *this );
      tmp += i;
      return tmp;
    }

    _iterator& operator-=( int i )
    {
      m_index -= i;
      return *this;
    }

    _iterator operator-( int i ) const
    {
      _iterator tmp( *this );
      tmp -= i;
      return tmp;
    }

    // dereference operator will return a ReferenceProxy object
    reference operator*() const
    {
      //return reference( m_ds_id, m_index, &m_iface );
      return reference( m_ds_id, m_index );
    }

}; // end class _iterator

#endif
