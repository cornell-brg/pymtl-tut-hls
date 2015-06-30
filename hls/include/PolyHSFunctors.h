//----------------------------------------------------------------------
// Contains polymorphic classes which are passed to user functions
// such as find_if
//----------------------------------------------------------------------

#ifndef POLYHS_FUNCTORS_H
#define POLYHS_FUNCTORS_H

#include "Types.h"
#include "PolyHSValue.h"

extern PeIfaceType g_pe_iface;

class UnaryPredicate {
  public:
    bool operator() ( const PolyHSValue& P ) const {
      switch ( P.type() ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
          return ap_int<32>(P.data[0]) == 0;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
          return ap_uint<32>(P.data[0]) == 0;
      };
      if ( P.type() == TYPE_POINT ) {
        pe_write( g_pe_iface, P.data[1] );  // point.x
        pe_write( g_pe_iface, P.data[2] );  // point.y
        PeRespMsg resp = pe_read( g_pe_iface );
        return (resp.data != 0);
      }
      return false;
    }
};

#endif
