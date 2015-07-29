//----------------------------------------------------------------------
// Contains polymorphic classes which are passed to user functions
// such as find_if
//----------------------------------------------------------------------

#ifndef POLYHS_FUNCTORS_H
#define POLYHS_FUNCTORS_H

#include "Types.h"
#include "PolyHSValue.h"

bool eqzero( const PolyHSValue& P ) {
  switch ( P.type() ) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
      return ap_int<32>(P.data[0]) == 0;
    case TYPE_UCHAR:
    case TYPE_USHORT:
    case TYPE_UINT:
      return ap_uint<32>(P.data[0]) == 0;
    default:
      break;
  }
  return false;
}

bool gtzero( const PolyHSValue& P ) {
  switch ( P.type() ) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
      return ap_int<32>(P.data[0]) > 0;
    case TYPE_UCHAR:
    case TYPE_USHORT:
    case TYPE_UINT:
      return ap_uint<32>(P.data[0]) > 0;
    default:
      break;
  }
  return false;
}

bool ltzero( const PolyHSValue& P ) {
  switch ( P.type() ) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
      return ap_int<32>(P.data[0]) < 0;
    case TYPE_UCHAR:
    case TYPE_USHORT:
    case TYPE_UINT:
      return ap_uint<32>(P.data[0]) < 0;
    default:
      break;
  }
  return false;
}


//---------------------------------------------------------
// UnaryPredicate
//---------------------------------------------------------
class PolyHSUnaryPredicate {
  ap_uint<8> m_pred;
  PeIfaceType& m_iface;

  public:
    // Constructor
    PolyHSUnaryPredicate( PeIfaceType& iface, const ap_uint<8> pred ) : 
      m_iface( iface ), m_pred( pred )
    { }
    PolyHSUnaryPredicate( const PolyHSUnaryPredicate& p ) :
      m_pred( p.m_pred )
    { }

    // Copy Constructor
    PolyHSUnaryPredicate( const PolyHSUnaryPredicate& P ) :
      m_iface( P.m_iface ), m_pred( P.m_pred )
    {}

    // Evalulation Operator
    bool operator() ( const PolyHSValue& P ) const {
      PeRespMsg resp;
      switch ( P.type() ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          switch ( m_pred ) {
            case 0:
              return ap_int<32>(P.data[0]) > 0;
            case 1:
              return ap_int<32>(P.data[0]) < 0;
            case 2:
              return ap_int<32>(P.data[0]) == 0;
            case 3:
              return ap_int<32>(P.data[0]) % 2 == 1;
            case 4:
              return ap_int<32>(P.data[0]) % 2 == 0;
          }
          break;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          switch ( m_pred ) {
            case 0:
              return ap_uint<32>(P.data[0]) > 0;
            case 1:
              return ap_uint<32>(P.data[0]) < 0;
            case 2:
              return ap_uint<32>(P.data[0]) == 0;
            case 3:
              return ap_uint<32>(P.data[0]) % 2 == 1;
            case 4:
              return ap_uint<32>(P.data[0]) % 2 == 0;
          }
          break;
        case TYPE_POINT:
          pe_write( m_iface, P.data[1] );  // point.x
          pe_write( m_iface, P.data[2] );  // point.y
          resp = pe_read( m_iface );
          return (resp.data != 0);
          break;
        default:
          break;
      };
      return false;
    }
};

class PolyHSUnaryOperator {
  ap_uint<8> m_op;

  public:
    // Constructors
    PolyHSUnaryOperator( const ap_uint<8> op ) : 
      m_op( op )
    {}
    PolyHSUnaryOperator( const PolyHSUnaryOperator& p ) :
      m_op( p.m_op )
    {}

    PolyHSValue operator() ( const PolyHSValue& P ) const {
      PolyHSValue P2( P );
      PeRespMsg resp;
      ap_uint<32> d;
      switch ( P.type() ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          //switch ( m_op ) {
          //};
          d = P.data[0];
          P2.data[0] = d*d;
          break;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          //switch ( m_op ) {
          //};
          d = P.data[0];
          P2.data[0] = d*d;
          break;
        case TYPE_POINT:
          /*P2.data[0] = P.data[0];
          pe_write( g_pe_iface, 2);           // number of args
          pe_write( g_pe_iface, P.data[1] );  // point.x
          pe_write( g_pe_iface, P.data[2] );  // point.y
          resp = pe_read( g_pe_iface );
          P2.data[1] = resp.data;
          resp = pe_read( g_pe_iface );
          P2.data[2] = resp.data;*/
          break;
        case TYPE_RGBA:
          pe_write( g_pe_iface, P.data[0] );
          resp = pe_read( g_pe_iface );
          P2.data[0] = resp.data;
          break;
        default:
          break;
      }
      return P2;
    }
};

#endif
