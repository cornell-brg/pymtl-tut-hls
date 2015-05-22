#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"
#define N 10

#include "../include/Polytype.h"
#include "../include/common.h"

typedef _iterator<Polytype> iterator;

// mark this as volatile to enforce stores/loads
volatile DtuIfaceType g_dtu_iface;

typedef ap_uint<3> PredicateType;

// ------------------------------------------------------------------
// Polymorphic User Algorithm
// findif
// ------------------------------------------------------------------
template <typename T>
_iterator<T> findif (_iterator<T> begin, _iterator<T> end, PredicateType pred_val) {
  for (; begin != end; ++begin) {
    T temp = *begin;
    switch (pred_val) {
      case 0:
        if (temp > 1) return begin;
        break;
      case 1:
        if (temp < 1) return begin;
        break;
      case 2:
        if (temp == 0) return begin;
        break;
    };
  }
  return begin;
}

// ------------------------------------------------------------------
// Processor Interface
// This function takes care of the accelerator interface to the
// processor, and calls the user algorithm
// ------------------------------------------------------------------
void top (volatile AcIfaceType &ac, volatile MemIfaceType &mem)
{
  static AcDataType s_first_ds_id;
  static AcDataType s_first_index;
  static AcDataType s_last_ds_id;
  static AcDataType s_last_index;
  static PredicateType s_pred;
  static AcDataType s_dt_desc_ptr;
  static AcDataType s_result;

  AcReqType req = ac.req;
  AcRespType resp;

  // handle write request
  if (AC_REQ_TYPE(req) != 0) {
    // call the accelerator
    if (AC_REQ_ADDR(req) == 0) {
      //DtuRespType dr = dtu_read(g_dtu_iface, s_first_ds_id, s_first_index);
      
      // read the metadata from memory
      MetaData metadata;
      /*unsigned md[MAX_FIELDS];
      SET_OFFSET( md[0], dr );
      SET_SIZE  ( md[0], sizeof(int) );
      SET_TYPE  ( md[0], TypeEnum<int>::get() );
      SET_FIELDS( md[0], 0 );
      metadata.init(md);*/
      mem_read_metadata (mem, s_dt_desc_ptr, metadata);

      /*printf ("%u %u\n%u %u\n", (unsigned)s_first_ds_id, (unsigned)s_first_index, 
                                (unsigned)s_last_ds_id,  (unsigned)s_last_index);*/
      s_result = findif<Polytype> (
                   iterator(s_first_ds_id, s_first_index, metadata),
                   iterator(s_last_ds_id, s_last_index, metadata),
                   s_pred
                 ).get_index();
    }
    else {
      // write internal regs
      switch (AC_REQ_ADDR(req)) {
        case 1:
          s_first_ds_id = AC_REQ_DATA(req); break;
        case 2:
          s_first_index = AC_REQ_DATA(req); break;
        case 3:
          s_last_ds_id = AC_REQ_DATA(req); break;
        case 4:
          s_last_index = AC_REQ_DATA(req); break;
        case 5:
          s_pred = AC_REQ_DATA(req); break;
        case 6:
          s_dt_desc_ptr = AC_REQ_DATA(req); break;
        default:
          break;
      }
    }
  
    // this is a dummy block to make sure the input/output
    // type of the mem iface is generated correctly
    if ( AC_REQ_ADDR(req) == 11 ) {
      MemReqType mreq = 0;
      mem.req = mreq;
      MemRespType mresp = mem.resp;
    }

    // ID, data, RW
    resp = make_ac_resp( AC_REQ_ID(req), 0, 1 );
  }
  // handle read request
  else {
    AcDataType data = 0;

    switch (AC_REQ_ADDR(req)) {
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

    // ID, data, RW
    resp = make_ac_resp( AC_REQ_ID(req), data, 0 );
  }

  ac.resp = resp;
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
void print_req (AcReqType req) {
  //printf ("req:  %s\n", req.to_string(16).c_str());
}
void print_resp (AcRespType resp) {
  //printf ("resp: %s\n", resp.to_string(16).c_str());
}

bool check_resp (AcRespType resp) {
  if (AC_RESP_TYPE(resp) != 1) return false;
  return true;
}

int main () {
  AcIfaceType ac_iface;
  MemIfaceType mem_iface;
  AcDataType data;
  AcAddrType raddr;
  MetaData* m = MetaCreator<unsigned>::get();

  AcIdType id = 0;

  // set first ds id
  data = 0;   raddr = 1;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );
  
  // set first index
  data = 0;   raddr = 2;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // set last ds id
  data = 0;   raddr = 3;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // set last index
  data = 7;   raddr = 4;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // set metadata pointer
  data = m;   raddr = 4;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // set pred
  data = 2;   raddr = 5;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // start accelerator
  data = 0;   raddr = 0;
  ac_iface.req = make_ac_req( id, data, raddr, 1 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);
  assert( check_resp(ac_iface.resp) );

  // read result
  data = 0;   raddr = 0;
  ac_iface.req = make_ac_req( id, data, raddr, 0 );
  print_req (ac_iface.req);
  top( ac_iface, mem_iface );
  print_resp (ac_iface.resp);

  unsigned s = AC_RESP_DATA(ac_iface.resp);
  printf ("--------------------\n");
  printf ("Result: %X\n", s);
  printf ("--------------------\n");
  //assert (s == 6);

  return 0;
}

