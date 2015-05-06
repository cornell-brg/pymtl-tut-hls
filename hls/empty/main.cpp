#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"
#define N 10

#include "../include/common.h"

typedef char MyType;
typedef _iterator<MyType> iterator;

// mark this as volatile to enforce stores/loads
volatile DtuIfaceType g_dtu_iface;

typedef ap_uint<3> PredicateType;

unsigned findif (iterator begin, iterator end, PredicateType pred_val);

// ------------------------------------------------------------------
// processor interface
// ------------------------------------------------------------------
void top (
    volatile AcReqType  &cfg_req, volatile AcRespType  &cfg_resp,
    volatile MemReqType &mem_req, volatile MemRespType &mem_resp
)
{
  static AcDataType s_first_ds_id;
  static AcDataType s_first_index;
  static AcDataType s_last_ds_id;
  static AcDataType s_last_index;
  static PredicateType s_pred;
  static AcDataType s_result;

  AcReqType req = cfg_req;
  AcRespType resp;

  // handle write request
  if (AC_REQ_TYPE(req) != 0) {
    // call the accelerator
    if (AC_REQ_ADDR(req) == 0) {
      /*printf ("%u %u\n%u %u\n", (unsigned)s_first_ds_id, (unsigned)s_first_index, 
                                (unsigned)s_last_ds_id,  (unsigned)s_last_index);*/
      s_result = findif (
                   iterator(s_first_ds_id, s_first_index),
                   iterator(s_last_ds_id, s_last_index),
                   s_pred
                 );
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
        default:
          break;
      }
    }
  
    if ( AC_REQ_ADDR(req) == 7) {
      MemReqType mreq = 0;
      mem_req = mreq;
      MemRespType mresp = mem_resp;
    }

    // ID, data, RW
    resp = make_resp( AC_REQ_ID(req), 0, 1);
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
      default:
        break;
    }

    // ID, data, RW
    resp = make_resp( AC_REQ_ID(req), data, 0);
  }

  cfg_resp = resp;
}

// ------------------------------------------------------------------
// findif logic
// ------------------------------------------------------------------
unsigned findif (iterator begin, iterator end, PredicateType pred_val) {
  int temp = *begin;
  *end = temp+1;
  return 6;
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
  MyType myarray[N];
  for (int i = 0; i < N; ++i) myarray[i] = i-3;

  AcRespType  resp;
  AcReqType   req;
  MemReqType  mreq;
  MemRespType mresp;
  AcDataType data;
  AcAddrType raddr;

  AcIdType id = 0;

  // set first ds id
  data = 0;   raddr = 1;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );
  
  // set first index
  data = 0;   raddr = 2;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set last ds id
  data = 0;   raddr = 3;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set last index
  data = 7;   raddr = 4;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set pred
  data = 4;   raddr = 5;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );

  // start accelerator
  data = 0;   raddr = 0;
  req = make_req( id, data, raddr, 1 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);
  assert( check_resp(resp) );

  // read result
  data = 0;   raddr = 0;
  req = make_req( id, data, raddr, 0 );
  print_req (req);
  top( req, resp, mreq, mresp );
  print_resp (resp);

  unsigned s = AC_RESP_DATA(resp);
  printf ("--------------------\n");
  printf ("Result: %X\n", s);
  printf ("--------------------\n");
  //assert (s == 6);

  return 0;
}

