#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"

#include "../hls/include/PolyHSFunctors.h"
#include "../hls/include/common.h"

DstuIfaceType g_dstu_iface;
PeIfaceType g_pe_iface;

// ------------------------------------------------------------------
// Polymorphic User Algorithm
// transform
// ------------------------------------------------------------------
template <typename Iterator, typename UnaryOperator>
Iterator transform(Iterator first1, Iterator last1, Iterator first2, UnaryOperator u_op) {
  while (first1 != last1) {
    *first2++ = u_op( *first1++ );
  }
  return first2;
}

// ------------------------------------------------------------------
// TransformUnitHLS Wrapper
// This function takes care of the accelerator interface to the
// processor, and calls the user algorithm
// FIXME: should return an iterator not an index
// ------------------------------------------------------------------
void TransformUnitHLS (AcIfaceType &ac, MemIfaceType &mem)
{
  // state variables
  static AcDataType s_first_ds_id;
  static AcDataType s_first_index;
  static AcDataType s_last_ds_id;
  static AcDataType s_last_index;
  static AcDataType s_first2_ds_id;
  static AcDataType s_first2_index;
  static AcDataType s_operator;
  static AcDataType s_dt_desc_ptr;
  static AcDataType s_result;

  AcReqMsg req;
  AcRespMsg resp;
  MetaData metadata;

  // 1. First ds id
  ac.req.read( req );
  s_first_ds_id = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 2. First index
  ac.req.read( req );
  s_first_index = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 3. Last ds id
  ac.req.read( req );
  s_last_ds_id = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 4. Last index
  ac.req.read( req );
  s_last_index = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 5. First2 ds id
  ac.req.read( req );
  s_first2_ds_id = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 6. First2 index
  ac.req.read( req );
  s_first2_index = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 7. Operator
  ac.req.read( req );
  s_operator = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 8. metadata ptr
  ac.req.read( req );
  s_dt_desc_ptr = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 9. start
  ac.req.read( req );
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );
  
  // Read metadata
  #ifdef CPP_COMPILE
    unsigned md[MAX_FIELDS];
    // descripter for RGBA
    SET_OFFSET( md[0], 0               );
    SET_SIZE  ( md[0], sizeof( int )   );
    SET_TYPE  ( md[0], TYPE_RGBA       );
    SET_FIELDS( md[0], 0               );
    metadata.init(md);
  #else
    mem_read_metadata (mem, s_dt_desc_ptr, metadata);
  #endif

  unsigned md0 = metadata.getData(0);
  ap_uint<8> type = GET_TYPE(md0);
  ap_uint<8> fields = GET_FIELDS(md0);

  // construct iterators and call the algorithm
  PolyHSIterator first1(s_first_ds_id, s_first_index, type, fields);
  PolyHSIterator last1(s_last_ds_id, s_last_index, type, fields);
  PolyHSIterator first2(s_first2_ds_id, s_first2_index, type, fields);

  s_result = transform(
                first1,
                last1,
                first2,
                PolyHSUnaryOperator( s_operator )
             ).get_index();

  // 8. Return result
  ac.req.read( req );
  ac.resp.write( AcRespMsg( req.id, s_result, req.type, req.opq ) );
}

// ------------------------------------------------------------------
// dstu model for simulation
// ------------------------------------------------------------------

// dstu_preprocess
//  Writes [n] pieces of data into the dstu resp queue
//  Simulates [n] dstu data requests
void dstu_preprocess( AcIdType id, const DstuDataType n ) {
  // id, data, rw type, opaque
  for (int i = 1; i <= n; ++i) {
    unsigned k = i+1;
    // respond to read
    g_dstu_iface.resp.write( DstuRespMsg( id, k, MSG_READ, 0 ));
    // respond to write
    g_dstu_iface.resp.write( DstuRespMsg( id, 0, MSG_WRITE, 0 ));
  }
}

// dstu_postprocess
//  Reads [n] pieces of data from the dstu req queue
//  Simulates [n] dstu data requests
void dstu_postprocess( const DstuDataType n ) {
  DstuReqMsg req;
  for (int i = 1; i <= n; ++i) {
    // read request
    g_dstu_iface.req.read( req );
    printf ("dstu req: %s %x\n", req.type ? "write" : " read", (unsigned)req.data);
    // write request
    g_dstu_iface.req.read( req );
    printf ("dstu req: %s %x\n", req.type ? "write" : " read", (unsigned)req.data);
  }
}

// ------------------------------------------------------------------
// PE model for simulation
// ------------------------------------------------------------------

// pe_postprocess
//  Writes [n] points to the pe resp queue
void pe_preprocess( AcIdType id, const PeDataType n ) {
  for (int i = 1; i <= n; ++i) {
    // respond to data write
    g_pe_iface.resp.write( PeRespMsg( id, MSG_WRITE, 0 ) );

    unsigned k = i + 1;

    // write back result
    g_pe_iface.resp.write( PeRespMsg( id, MSG_READ, k*k ) );
  }
}

// pe_postprocess
//  Reads [n] piece of data from the pe req queue
void pe_postprocess( const PeDataType n ) {
  PeReqMsg req;
  for (int i = 1; i <= n; ++i) {
    g_pe_iface.req.read( req );
    printf ("pe req: %s %x\n", req.type ? "write" : " read", (unsigned)req.data);
    g_pe_iface.req.read( req );
    printf ("pe req: %s %x\n", req.type ? "write" : " read", (unsigned)req.data);
  }
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
bool test_transform( const unsigned n ) {
  AcIfaceType ac_iface;
  MemIfaceType mem_iface;
  AcDataType data;
  AcAddrType raddr;
  MetaData* m = MetaCreator<unsigned>::get();
  AcRespMsg resp;

  AcIdType id = 0;

  // 1. set first ds id
  data = 0;   raddr = 1;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 2. set first index
  data = 0;   raddr = 2;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 3. set last ds id
  data = 0;   raddr = 3;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 4. set last index
  data = n; raddr = 4;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 5. set first2 ds id
  data = 0;   raddr = 3;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 6. set first2 index
  data = 0; raddr = 4;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 7. set pred
  data = 2;   raddr = 5;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 8. set metadata pointer
  data = m;   raddr = 6;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 9. start accelerator
  data = 0;   raddr = 0;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  // 10. read result
  data = 0;   raddr = 0;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_READ, 0 ) );

  dstu_preprocess( id, n );
  pe_preprocess  ( id, n );

  TransformUnitHLS( ac_iface, mem_iface );
  
  pe_postprocess  ( n );
  dstu_postprocess( n );

  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  ac_iface.resp.read( resp );
  
  //printf ("--------------------\n");
  //printf ("Expected : %X\n", n);
  //printf ("Result   : %X\n", s);
  //printf ("--------------------\n");

  return true;
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main () {
  for (int i = 0; i < 10; ++i) {
    assert( test_transform( i ) );
  }
  printf ("All tests passed\n");
  return 0;
}

