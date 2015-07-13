#include "ap_utils.h"
#include "assert.h"

#include "../hls/include/Types.h"

// ------------------------------------------------------------------
// Specialized Processing Element
// Calculate Square of input
// ------------------------------------------------------------------
void Square( PeIfaceType &pe ) {
  PeReqMsg  req;
  PeDataType x,y;
  bool result = true;
  PeDataType m_data[10];

  // read number of args
  pe.req.read( req );
  PeDataType n_args = req.data;
  pe.resp.write( PeRespMsg( req.id, req.type, 0 ) );

  // read args
  for (PeDataType i = 0; i < n_args; ++i) {
    pe.req.read( req );
    m_data[i] = req.data;
    pe.resp.write( PeRespMsg( req.id, req.type, 0 ) );
  }

  // return result
  for (PeDataType i = 0; i < n_args; ++i) {
    pe.req.read( req );
    PeDataType temp = m_data[i];
    pe.resp.write( PeRespMsg( req.id, req.type, temp*temp ) );
  }
}

// ------------------------------------------------------------------
// ASU model for simulation
// ------------------------------------------------------------------

// asu_postprocess
//  Tests the PE with n data points
//  Writes the n values to the PE
void asu_preprocess( PeIfaceType& iface, PeIdType id, const int n ) {
  // write num of args 
  iface.req.write( PeReqMsg( id, MSG_WRITE, n ) );
  
  // write data
  for (int i = 0; i < n; ++i) {
    iface.req.write( PeReqMsg( id, MSG_WRITE, i ) );
  }
  
  // read results
  for (int i = 0; i < n; ++i) {
    iface.req.write( PeReqMsg( id, MSG_READ, 0 ) );
  }
}

// asu_postprocess
//  Tests the PE with n data points
//  Reads n values from the PE
void asu_postprocess( PeIfaceType& iface, const int n ) {
  PeRespMsg resp;
  iface.resp.read( resp );
  
  for (int i = 0; i < n; ++i) {
    iface.resp.read( resp );
    assert( resp.data == 0 );
  }
  
  for (int i = 0; i < n; ++i) {
    iface.resp.read( resp );
    assert( resp.data == i*i );
  }
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
bool test_eqzero( const unsigned n ) {
  PeIfaceType iface;
  PeIdType id = 0;

  asu_preprocess( iface, id, n );

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

