#ifndef RGBA_H
#define RGBA_H

#include "../ReferenceProxy.h"
#include "../MetaData.h"
#include "../TypeEnum.h"

//----------------------------------------------------------------------
// RGBA type
//----------------------------------------------------------------------
struct RGBA {
  public:
    //------------------------------------------------------------------
    // Pack 4 bytes into a 32-bit value
    //------------------------------------------------------------------
    ap_uint<32> data;
    
    //------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------
    RGBA() : data( 0 ) {}
    RGBA( ap_uint<32> d ) : data( d ) {}

    //------------------------------------------------------------------
    // Reading each byte
    //------------------------------------------------------------------
    ap_uint<8> getR() const { return data(0,7);   }
    ap_uint<8> getG() const { return data(8,15);  }
    ap_uint<8> getB() const { return data(16,23); }
    ap_uint<8> getA() const { return data(24,31); }

    //------------------------------------------------------------------
    // Writing each byte
    //------------------------------------------------------------------
    void setR( ap_uint<8> x ) { data(0,7) = x;   }
    void setG( ap_uint<8> x ) { data(8,15) = x;  }
    void setB( ap_uint<8> x ) { data(16,23) = x; }
    void setA( ap_uint<8> x ) { data(24,31) = x; }
};

//----------------------------------------------------------------------
// MetaCreator - RGBA Specialization
//----------------------------------------------------------------------
template<>
class MetaCreator <RGBA> {
  public:
    MetaCreator() {
      unsigned int data[MAX_FIELDS];

      // descripter for RGBA
      SET_OFFSET( data[0], 0               );
      SET_SIZE  ( data[0], sizeof( int )   );
      SET_TYPE  ( data[0], TYPE_RGBA       );
      SET_FIELDS( data[0], 0               );

      get()->init( data );
    }

    static MetaData* get() {
      static MetaData instance;
      return &instance;
    }
};

#endif
