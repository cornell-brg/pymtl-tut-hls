//----------------------------------------------------------------------
// The PolyHSValue class is a polymorphic data type for synthesis 
// purposes. It can represent any primitive type or user struct
// with the help of the Metadata.
//----------------------------------------------------------------------

#ifndef POLYHS_VALUE_H
#define POLYHS_VALUE_H

#include "Types.h"

//----------------------------------------------------------------------
// PolyHSValue
//----------------------------------------------------------------------
class PolyHSValue {
  private:
    const ap_uint<8> m_type;

  public:
    unsigned data[MAX_FIELDS];

  public:
    //------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------
    PolyHSValue() : m_type( 0 ) {}
    PolyHSValue( const ap_uint<8> type ) : m_type( type ) {}
    PolyHSValue( const PolyHSValue& p ) : m_type( p.m_type ) {}
    
    ap_uint<8> type() const { return m_type; }
    
    //------------------------------------------------------------------
    // Boolean Operators against Integer
    //------------------------------------------------------------------

    bool operator==(const int rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          return ap_int<32>(data[0]) == ap_int<32>(rhs);
        case TYPE_UCHAR:
        case TYPE_USHORT:
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
        case TYPE_SHORT:
        case TYPE_INT:
          return ap_int<32>(data[0]) <= ap_int<32>(rhs);
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          return ap_uint<32>(data[0]) <= ap_uint<32>(rhs);
        default:
          break;
      }

      if (m_type == TYPE_POINT) {
          // RZ: vivado_hls produces an error if we use pointer
          // typecast for this operator
          Point t;
          t.label = data[0];
          t.x = data[1];
          t.y = data[2];
          return t <= rhs;
      }

      return false; 
    }

    bool operator<(const int rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          return ap_int<32>(data[0]) < ap_int<32>(rhs);
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          return ap_uint<32>(data[0]) < ap_uint<32>(rhs);
        default:
          break;
      }

      if (m_type == TYPE_POINT) {
          // RZ: vivado_hls produces an error if we use pointer
          // typecast for this operator
          Point t;
          t.label = data[0];
          t.x = data[1];
          t.y = data[2];
          return t < rhs;
      }

      return false; 
    }
    
    // derived operators
    bool operator> (const int rhs) const { return !(*this <= rhs); }
    bool operator>=(const int rhs) const { return !(*this <  rhs); }
    bool operator!=(const int rhs) const { return !(*this == rhs); }

    //------------------------------------------------------------------
    // Arithmetic Operators
    //------------------------------------------------------------------
    int operator %(const unsigned rhs) const {
      switch (m_type) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          return ap_int<32>(data[0]) % rhs;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          return  ap_uint<32>(data[0]) % rhs;
        default:
          break;
      }
      return 0;
    }
};

#endif
