//----------------------------------------------------------------------
// Polymorphic data type for synthesis purposes
//----------------------------------------------------------------------

#ifndef POLYTYPE_H
#define POLYTYPE_H

#include "ReferenceProxy.h"
#include "Iterator.h"
#include "MetaData.h"

#if 1
  #define DB( x ) x
#else
  #define DB( x )
#endif

extern volatile DtuIfaceType g_dtu_iface;

//----------------------------------------------------------------------
// The Polytype class is meant to represent any struct or
// primitive class
//----------------------------------------------------------------------
class Polytype {
  private:
    const MetaData& m_metadata;

  public:
    unsigned data[MAX_FIELDS];

  public:
    //------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------
    Polytype() : m_metadata( MetaData() ) {}
    Polytype( const MetaData& md ) : m_metadata( md ) {}
    Polytype( const Polytype& p ) : m_metadata( p.m_metadata ) {
      for (int i = 0; i < MAX_FIELDS; ++i)
        data[i] = p.data[i];
    }
    
    //------------------------------------------------------------------
    // Helpers
    //------------------------------------------------------------------
    
    //------------------------------------------------------------------
    // Boolean Operators against Integer
    //------------------------------------------------------------------

    bool operator==(const int rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }

    bool operator<=(const int rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }

    bool operator<(const int rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }
    
    // derived operators
    bool operator> (const int rhs) const { return !(*this <= rhs); }
    bool operator>=(const int rhs) const { return !(*this < rhs); }
    bool operator!=(const int rhs) const { return !(*this == rhs); }

    //------------------------------------------------------------------
    // Boolean Operators against Polytype
    //------------------------------------------------------------------
    // RZ: for all operators we assume the RHS has the same actual type

    // basic operators
    bool operator==(const Polytype& rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }

    bool operator<=(const Polytype& rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }
    bool operator< (const Polytype& rhs) const {
      unsigned md0 = m_metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      switch (type) {
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
        default:
          break;
      }

      return false; 
    }
    
    // derived operators
    bool operator> (const Polytype& rhs) const { return !(*this <= rhs); }
    bool operator>=(const Polytype& rhs) const { return !(*this < rhs); }
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
    DtuIdType m_ds_id;

    // index to an data element
    DtuIterType m_iter;

    // metadata
    const MetaData& m_metadata;

  public:

    // Constructors
    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ), m_metadata ( MetaData() ) {}
    ReferenceProxy( DtuIdType ds_id, DtuIterType iter, const MetaData& md )
      : m_ds_id( ds_id ), m_iter( iter ), m_metadata( md ) {}

    ~ReferenceProxy(){}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object

    operator Polytype() const
    {
      Polytype p (m_metadata);
      DtuRespType resp;
      
      unsigned md = m_metadata.getData(0);
      ap_uint<8> n_fields = GET_FIELDS(md);

      // primitive type
      if (n_fields == 0) {
        resp = dtu_read (g_dtu_iface, m_ds_id, m_iter);
        p.data[0] = DTU_RESP_DATA(resp);
      }
      // struct
      else {
        for (int i = 1; i < MAX_FIELDS; ++i) {
          if (i < n_fields) {
            resp = dtu_read (g_dtu_iface, m_ds_id, m_iter, i);
            p.data[i] = DTU_RESP_DATA(resp);
          }
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
      DtuRespType resp;
      DtuDataType data;
      
      unsigned md = m_metadata.getData(0);
      ap_uint<8> n_fields = GET_FIELDS(md);

      // primitive type
      if (n_fields == 0) {
        resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, 0, p.data[0]);
      }
      // struct
      else {
        for (int i = 1; i < MAX_FIELDS; ++i) {
          if (i < n_fields) {
            resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, i, p.data[i]);
          }
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
    const MetaData& m_metadata;

  public:

    //--------------------------------------------------------------
    //  constructors
    //--------------------------------------------------------------

    // default constructor
    _iterator() : m_ds_id( 0 ), m_index( 0 ), m_metadata( MetaData() ) {}

    _iterator( unsigned ds_id, unsigned index, const MetaData& md ) :
      m_ds_id( ds_id ), m_index( index ), m_metadata( md ) {}

    // copy constructor
    _iterator( const _iterator& it )
      : m_ds_id( it.m_ds_id ), m_index( it.m_index ), m_metadata( it.m_metadata ) {}

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
      return reference( m_ds_id, m_index, m_metadata );
    }

}; // end class _iterator

#endif
