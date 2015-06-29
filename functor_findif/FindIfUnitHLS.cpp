#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"

#include "../hls/include/PolyHSFunctors.h"
#include "../hls/include/common.h"

DstuIfaceType g_dstu_iface;
PeIfaceType   g_pe_iface;

// ------------------------------------------------------------------
// Polymorphic User Algorithm
// findif
// ------------------------------------------------------------------
template <typename Iterator>
Iterator findif (Iterator begin, Iterator end, UnaryPredicate pred) {
  for (; begin != end; ++begin) {
    typename Iterator::value_type temp = *begin;
    if ( pred( temp ) )
      return begin;
  }
  return begin;
}

// ------------------------------------------------------------------
// Wrapper
// This function takes care of the accelerator interface to the
// processor, and calls the user algorithm
// ------------------------------------------------------------------
void FindIfUnitHLS (AcIfaceType &ac, MemIfaceType &mem)
{
  // state variables
  static AcDataType s_first_ds_id;
  static AcDataType s_first_index;
  static AcDataType s_last_ds_id;
  static AcDataType s_last_index;
  static AcDataType s_pred;
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

  // 5. Predicate
  ac.req.read( req );
  s_pred = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 6. metadata ptr
  ac.req.read( req );
  s_dt_desc_ptr = req.data;
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );

  // 7. start
  ac.req.read( req );
  ac.resp.write( AcRespMsg( req.id, 0, req.type, req.opq ) );
  ap_wait();

  // Compute
  #if 1
    unsigned md[MAX_FIELDS];
    /*// descriptor for point
    SET_OFFSET( md[0], 0               );
    SET_SIZE  ( md[0], sizeof( Point ) );
    SET_TYPE  ( md[0], TYPE_POINT      );
    SET_FIELDS( md[0], 3               );
    // descriptor for label
    SET_OFFSET( md[1], 0               );
    SET_SIZE  ( md[1], sizeof( int   ) );
    SET_TYPE  ( md[1], TYPE_SHORT      );
    SET_FIELDS( md[1], 0               );
    // descriptor for x
    SET_OFFSET( md[2], 4               );
    SET_SIZE  ( md[2], sizeof( int   ) );
    SET_TYPE  ( md[2], TYPE_INT        );
    SET_FIELDS( md[2], 0               );
    // descriptor for y
    SET_OFFSET( md[3], 8               );
    SET_SIZE  ( md[3], sizeof( int   ) );
    SET_TYPE  ( md[3], TYPE_INT        );
    SET_FIELDS( md[3], 0               );*/
    SET_OFFSET( md[0], 0               );
    SET_SIZE  ( md[0], sizeof( int   ) );
    SET_TYPE  ( md[0], TYPE_INT        );
    SET_FIELDS( md[0], 0               );

    metadata.init(md);
  #else
    mem_read_metadata (mem, s_dt_desc_ptr, metadata);
  #endif

  unsigned md0 = metadata.getData(0);
  ap_uint<8> type = GET_TYPE(md0);
  ap_uint<8> fields = GET_FIELDS(md0);

  s_result = findif (
               PolyHSIterator(s_first_ds_id, s_first_index, type, fields),
               PolyHSIterator(s_last_ds_id, s_last_index, type, fields),
               UnaryPredicate()
             ).get_index();

  // 8. Return result
  ac.req.read( req );
  ac.resp.write( AcRespMsg( req.id, s_result, req.type, req.opq ) );
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
bool check_resp (AcRespMsg resp) {
  if (resp.type != 1) return false;
  return true;
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main () {
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
  data = 7;   raddr = 4;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // 5. set metadata pointer
  data = m;   raddr = 4;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // 6. set pred
  data = 2;   raddr = 5;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // 7. start accelerator
  data = 0;   raddr = 0;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // 8. read result
  data = 0;   raddr = 0;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_READ, 0 ) );

  FindIfUnitHLS( ac_iface, mem_iface );
  
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );
  ac_iface.resp.read ( resp );

  unsigned s = resp.data;
  printf ("--------------------\n");
  printf ("Result: %X\n", s);
  printf ("--------------------\n");
  //assert (s == 6);

  return 0;
}

