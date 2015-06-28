#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"

#include "../include/common.h"

DstuIfaceType g_dstu_iface;

typedef ap_uint<3> PredicateType;

// ------------------------------------------------------------------
// Polymorphic User Algorithm
// empty
// ------------------------------------------------------------------
template <typename Iterator>
Iterator empty (Iterator begin, Iterator end, PredicateType pred_val) {
  typename Iterator::value_type temp = *begin;
  ++begin;
  *begin = temp;
  return begin;
}

// ------------------------------------------------------------------
// Processor Interface
// This function takes care of the accelerator interface to the
// processor, and calls the user algorithm
// ------------------------------------------------------------------
void top (AcIfaceType &ac, MemIfaceType &mem)
{
  static AcDataType s_first_ds_id;
  static AcDataType s_first_index;
  static AcDataType s_last_ds_id;
  static AcDataType s_last_index;
  static PredicateType s_pred;
  static AcDataType s_dt_desc_ptr;
  static AcDataType s_result;

  AcReqMsg req = ac.req.read();
  AcRespMsg resp;

  // handle write request
  if (req.type != MSG_READ) {
    // call the accelerator
    if (req.addr == 0) {
      // read the metadata from memory
      MetaData metadata;
    #if 0
      unsigned md[MAX_FIELDS];
      SET_OFFSET( md[0], 0 );
      SET_SIZE  ( md[0], sizeof(int) );
      SET_TYPE  ( md[0], TypeEnum<int>::get() );
      SET_FIELDS( md[0], 0 );
      metadata.init(md);
    #else
      mem_read_metadata (mem, s_dt_desc_ptr, metadata);
    #endif

      unsigned md0 = metadata.getData(0);
      ap_uint<8> type = GET_TYPE(md0);
      ap_uint<8> fields = GET_FIELDS(md0);

      s_result = empty<PolyHSIterator> (
                   PolyHSIterator(s_first_ds_id, s_first_index, type, fields),
                   PolyHSIterator(s_last_ds_id, s_last_index, type, fields),
                   s_pred
                 ).get_index();
    }
    else {
      // write internal regs
      switch (req.addr) {
        case 1:
          s_first_ds_id = req.data; break;
        case 2:
          s_first_index = req.data; break;
        case 3:
          s_last_ds_id = req.data; break;
        case 4:
          s_last_index = req.data; break;
        case 5:
          s_pred = req.data; break;
        case 6:
          s_dt_desc_ptr = req.data; break;
        default:
          break;
      }
    }
  
    // this is a dummy block to make sure the input/output
    // type of the mem iface is generated correctly
    /*if ( req.addr == 11 ) {
      mem.req.write( MemReqMsg() );
      MemRespMsg mresp = mem.resp.read();
    }*/

    // ID, data, RW, opq
    resp = AcRespMsg( req.id, 0, MSG_WRITE, 0 );
  }
  // handle read request
  else {
    AcDataType data = 0;

    switch (req.addr) {
      case 0:
        data = s_result; break;
      case 1:
        data = s_first_ds_id; break;
      case 2:
        data = s_first_index; break;
      case 3:
        data = s_last_ds_id; break;
      case 4:
        data = s_last_index; break;
      case 5:
        data = s_pred; break;
      case 6:
        data = s_dt_desc_ptr; break;
      default:
        break;
    }

    // ID, data, RW, opq
    resp = AcRespMsg( req.id, data, MSG_READ, 0 );
  }

  ap_wait();
  ac.resp.write( resp );
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
bool check_resp (AcRespMsg resp) {
  if (resp.type != 1) return false;
  return true;
}

void call_accel (AcIfaceType& ac, MemIfaceType& mem, AcReqMsg msg) {
  ac.req.write( msg );
  top ( ac, mem );
  assert( check_resp( ac.resp.read() ) );
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

  AcIdType id = 0;

  // set first ds id
  data = 0;   raddr = 1;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );
  
  // set first index
  data = 0;   raddr = 2;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // set last ds id
  data = 0;   raddr = 3;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // set last index
  data = 7;   raddr = 4;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // set metadata pointer
  data = m;   raddr = 4;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // set pred
  data = 2;   raddr = 5;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // start accelerator
  data = 0;   raddr = 0;
  call_accel( ac_iface, mem_iface, AcReqMsg( id, data, raddr, MSG_WRITE, 0 ) );

  // read result
  data = 0;   raddr = 0;
  ac_iface.req.write( AcReqMsg( id, data, raddr, MSG_READ, 0 ) );
  top( ac_iface, mem_iface );

  AcRespMsg resp = ac_iface.resp.read();
  unsigned s = resp.data;
  printf ("--------------------\n");
  printf ("Result: %X\n", s);
  printf ("--------------------\n");
  //assert (s == 6);

  return 0;
}

