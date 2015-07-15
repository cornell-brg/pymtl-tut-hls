#include "ap_utils.h"
#include "assert.h"

#include "../hls/include/Types.h"

RGBA rgb2cmyk( const RGBA rgb ) {
  ap_uint<8> R = rgb.getR();
  ap_uint<8> G = rgb.getG();
  ap_uint<8> B = rgb.getB();
  
  ap_uint<8> c = 255 - R;
  ap_uint<8> m = 255 - G;
  ap_uint<8> y = 255 - B;

  // K is the min of c,m,y
  ap_uint<8> K = (c < m ) ? (c < y ? c : y) : (m < y ? m : y);

  RGBA ret;
  ret.setR( c - K );
  ret.setG( m - K );
  ret.setB( y - K );
  ret.setA( K );
  return ret;
}

//------------------------------------------------------------------
// Specialized Processing Element
// Calculate Square of input
// ------------------------------------------------------------------
void Square( PeIfaceType &pe ) {
  PeReqMsg  req;
  PeDataType x,y;

  // read args
  pe.req.read( req );
  RGBA temp( req.data );
  pe.resp.write( PeRespMsg( req.id, req.type, 0 ) );

  RGBA result = rgb2cmyk( temp );

  // return result
  pe.req.read( req );
  pe.resp.write( PeRespMsg( req.id, req.type, result.data ) );
}

// ------------------------------------------------------------------
// ASU model for simulation
// ------------------------------------------------------------------

// asu_postprocess
//  Tests the PE with n data points
//  Writes the n values to the PE
void asu_preprocess( PeIfaceType& iface, PeIdType id, const int n ) {
  for (int i = 0; i < n; ++i) {
    // write data
    iface.req.write( PeReqMsg( id, MSG_WRITE, i ) );
    // read results
    iface.req.write( PeReqMsg( id, MSG_READ, 0 ) );
  }
}

// asu_postprocess
//  Tests the PE with n data points
//  Reads n values from the PE
void asu_postprocess( PeIfaceType& iface, const int n ) {
  PeRespMsg resp;
  
  for (int i = 0; i < n; ++i) {
    iface.resp.read( resp );
    assert( resp.data == 0 );
    iface.resp.read( resp );
  }
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
bool test_eqzero( const unsigned n ) {
  PeIfaceType iface;
  PeIdType id = 0;

  asu_preprocess( iface, id, n );

  for (unsigned i = 0; i < n; ++i)
    Square( iface );
  
  asu_postprocess( iface, n );

  // pass as long as we don't exit in asu_postprocess
  printf ("--------------------\n");
  printf ("Result   : Pass\n");
  printf ("--------------------\n");
  
  return true;
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main () {
  for (int i = 1; i <= 10; ++i) {
    assert( test_eqzero( i ) );
  }

  printf ("All tests passed\n");
  return 0;
}

