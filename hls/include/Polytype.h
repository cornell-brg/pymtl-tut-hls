//----------------------------------------------------------------------
// Polymorphic data type for synthesis purposes
//----------------------------------------------------------------------

#ifndef POLYTYPE_H
#define POLYTYPE_H

#include "ReferenceProxy.h"
#include "Iterator.h"
#include "MetaData.h"
#include "Types.h"

#if 0
  #define DB( x ) x
#else
  #define DB( x )
#endif

extern DstuIfaceType g_dstu_iface;

//----------------------------------------------------------------------
// The Polytype class is meant to represent any struct or
// primitive class
//----------------------------------------------------------------------
class Polytype {
  private:
    const ap_uint<8> m_type;

  public:
    unsigned data[MAX_FIELDS];

  public:
    //------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------
    Polytype() : m_type( 0 ) {}
    Polytype( const ap_uint<8> type ) : m_type( type ) {}
    Polytype( const Polytype& p ) : m_type( p.m_type ) {}
    
    //------------------------------------------------------------------
    // Helpers
    //------------------------------------------------------------------
    
    //------------------------------------------------------------------
    // Boolean Operators against Integer
    //------------------------------------------------------------------

    bool operator==(const int rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) == ap_int<8>(rhs);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) == ap_uint<8>(rhs);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) == ap_int<16>(rhs);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) == ap_uint<16>(rhs);
        case TYPE_INT:
          return ap_int<32>(data[0]) == ap_int<32>(rhs);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) == ap_uint<32>(rhs);
        case TYPE_POINT:
          return *((Point*)data) == rhs;
        default:
          break;
      }

      return false; 
    }

    bool operator<=(const int rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) <= ap_int<8>(rhs);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) <= ap_uint<8>(rhs);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) <= ap_int<16>(rhs);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) <= ap_uint<16>(rhs);
        case TYPE_INT:
          return ap_int<32>(data[0]) <= ap_int<32>(rhs);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) <= ap_uint<32>(rhs);
        case TYPE_POINT:
          // RZ: vivado_hls produces an error if we use pointer
          // typecast for this operator
          Point t;
          t.label = data[0];
          t.x = data[1];
          t.y = data[2];
          return t <= rhs;
        default:
          break;
      }

      return false; 
    }

    bool operator<(const int rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) < ap_int<8>(rhs);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) < ap_uint<8>(rhs);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) < ap_int<16>(rhs);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) < ap_uint<16>(rhs);
        case TYPE_INT:
          return ap_int<32>(data[0]) < ap_int<32>(rhs);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) < ap_uint<32>(rhs);
        case TYPE_POINT:
          // RZ: vivado_hls produces an error if we use pointer
          // typecast for this operator
          Point t;
          t.label = data[0];
          t.x = data[1];
          t.y = data[2];
          return t < rhs;
        default:
          break;
      }

      return false; 
    }
    
    // derived operators
    bool operator> (const int rhs) const { return !(*this <= rhs); }
    bool operator>=(const int rhs) const { return !(*this <  rhs); }
    bool operator!=(const int rhs) const { return !(*this == rhs); }

    //------------------------------------------------------------------
    // Boolean Operators against Polytype
    //------------------------------------------------------------------
    // RZ: for all operators we assume the RHS has the same actual type

    // basic operators
    bool operator==(const Polytype& rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) == ap_int<8>(rhs.data[0]);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) == ap_uint<8>(rhs.data[0]);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) == ap_int<16>(rhs.data[0]);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) == ap_uint<16>(rhs.data[0]);
        case TYPE_INT:
          return ap_int<32>(data[0]) == ap_int<32>(rhs.data[0]);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) == ap_uint<32>(rhs.data[0]);
        case TYPE_POINT:
          return *((Point*)data) == *((Point*)rhs.data);
        default:
          break;
      }

      return false; 
    }

    bool operator<=(const Polytype& rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) <= ap_int<8>(rhs.data[0]);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) <= ap_uint<8>(rhs.data[0]);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) <= ap_int<16>(rhs.data[0]);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) <= ap_uint<16>(rhs.data[0]);
        case TYPE_INT:
          return ap_int<32>(data[0]) <= ap_int<32>(rhs.data[0]);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) <= ap_uint<32>(rhs.data[0]);
        case TYPE_POINT:
          return *((Point*)data) <= *((Point*)rhs.data);
        default:
          break;
      }

      return false; 
    }
    bool operator< (const Polytype& rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
          return ap_int<8>(data[0]) < ap_int<8>(rhs.data[0]);
        case TYPE_UCHAR:
          return ap_uint<8>(data[0]) < ap_uint<8>(rhs.data[0]);
        case TYPE_SHORT:
          return ap_int<16>(data[0]) < ap_int<16>(rhs.data[0]);
        case TYPE_USHORT:
          return ap_uint<16>(data[0]) < ap_uint<16>(rhs.data[0]);
        case TYPE_INT:
          return ap_int<32>(data[0]) < ap_int<32>(rhs.data[0]);
        case TYPE_UINT:
          return ap_uint<32>(data[0]) < ap_uint<32>(rhs.data[0]);
        case TYPE_POINT:
          return *((Point*)data) < *((Point*)rhs.data);
        default:
          break;
      }

      return false; 
    }
    
    // derived operators
    bool operator> (const Polytype& rhs) const { return !(*this <= rhs); }
    bool operator>=(const Polytype& rhs) const { return !(*this <  rhs); }
    bool operator!=(const Polytype& rhs) const { return !(*this == rhs); }

    //------------------------------------------------------------------
    // Arithmetic Operators
    //------------------------------------------------------------------
};

//----------------------------------------------------------------------
// ReferenceProxy Polytype Specialization
//----------------------------------------------------------------------
template<>
class ReferenceProxy <Polytype> {
  private:
    // data structure id
    DstuIdType m_ds_id;

    // index to an data element
    DstuIterType m_iter;

    // metadata
    const ap_uint<8> m_type;
    const ap_uint<8> m_fields;

  public:

    // Constructors
    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ), m_type ( 0 ) {}
    ReferenceProxy( DstuIdType ds_id, DstuIterType iter, 
                    const ap_uint<8> type, const ap_uint<8> fields )
      : m_ds_id( ds_id ), m_iter( iter ), m_type( type ), m_fields( fields ) {}

    ~ReferenceProxy(){}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object

    operator Polytype() const
    {
      Polytype p (m_type);
      DstuRespMsg resp;
      
      // primitive type
      if (m_fields == 0) {
        resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, 0);
        p.data[0] = resp.data;
      }
      // struct
      else {
        for (int i = 0; i < m_fields; ++i) {
          resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, i+1);
          p.data[i] = resp.data;
        }
      }

      return p;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- type::value
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    ReferenceProxy& operator=( Polytype p )
    {
      DstuRespMsg resp;
      DstuDataType data;
      
      // primitive type
      if (m_fields == 0) {
        resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, 0, p.data[0]);
      }
      // struct
      else {
        for (int i = 0; i < m_fields; ++i) {
          resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, i+1, p.data[i]);
        }
      }

      // do nothing!
      return *this;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- ReferenceProxy argument
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    ReferenceProxy& operator=( const ReferenceProxy& x )
    {
      return operator=( static_cast<Polytype>( x ) );
    }
};

//-------------------------------------------------------------------
// Iterator Polytype specialization, identical except it uses metadata
//-------------------------------------------------------------------
template<>
class _iterator<Polytype> {

  public:
    //typedef std::random_access_iterator_tag   iterator_category;
    //typedef ptrdiff_t                         difference_type;
    typedef Polytype                          value_type;
    typedef _iterator                         pointer;
    typedef ReferenceProxy<Polytype>          reference;
    
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
    _iterator() : m_ds_id( 0 ), m_index( 0 ), m_type( 0 ) {}

    _iterator( unsigned ds_id, unsigned index, 
               const ap_uint<8> type, const ap_uint<8> fields ) :
      m_ds_id( ds_id ), m_index( index ), m_type( type ), m_fields( fields ) {}

    // copy constructor
    _iterator( const _iterator& it )
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
    _iterator& operator+=( unsigned i )
    {
      m_index += i;
      return *this;
    }

    _iterator operator+( unsigned i ) const
    {
      _iterator tmp( *this );
      tmp += i;
      return tmp;
    }

    _iterator& operator-=( unsigned i )
    {
      m_index -= i;
      return *this;
    }

    _iterator operator-( unsigned i ) const
    {
      _iterator tmp( *this );
      tmp -= i;
      return tmp;
    }

    // dereference operator will return a ReferenceProxy object
    reference operator*() const
    {
      return reference( m_ds_id, m_index, m_type, m_fields );
    }

}; // end class _iterator

#endif
