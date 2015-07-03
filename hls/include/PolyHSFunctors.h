//----------------------------------------------------------------------
// Contains polymorphic classes which are passed to user functions
// such as find_if
//----------------------------------------------------------------------

#ifndef POLYHS_FUNCTORS_H
#define POLYHS_FUNCTORS_H

#include "Types.h"
#include "PolyHSValue.h"

extern PeIfaceType g_pe_iface;

class PolyHSUnaryPredicate {
  ap_uint<8> m_pred;

  public:
    PolyHSUnaryPredicate( const ap_uint<8> pred ) : 
      m_pred( pred )
    { }

    bool operator() ( const PolyHSValue& P ) const {
      PeRespMsg resp;
      switch ( P.type() ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT: {
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
        }
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
          pe_write( g_pe_iface, P.data[1] );  // point.x
          pe_write( g_pe_iface, P.data[2] );  // point.y
          resp = pe_read( g_pe_iface );
          return (resp.data != 0);
          break;
        default:
          break;
      };
      return false;
    }
};

#endif
