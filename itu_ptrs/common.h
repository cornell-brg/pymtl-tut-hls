//========================================================================
// common.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : July 09, 2015

#ifndef COMMON_H
#define COMMON_H

#include <ap_int.h>

//------------------------------------------------------------------------
// dtValue
//------------------------------------------------------------------------

struct dtValue {
  ap_uint<8> offset;
  ap_uint<8> size;
  ap_uint<8> type;
  ap_uint<8> fields;

  dtValue() : fields(0), type(0), size(0), offset(0) {}

  dtValue( ap_uint<32> value ) {
    offset = ( value >> 24 ) & 0xff;
    size   = ( value >> 16 ) & 0xff;
    type   = ( value >> 8  ) & 0xff;
    fields = ( value & 0xff );
  }

};

#endif /*COMMON_H*/
