#include <assert.h>
#include <stdio.h>
#include "ap_utils.h"
#define N 10

#include "../common/common.h"

typedef char MyType;
typedef _iterator<MyType> iterator;

// mark this as volatile to enforce stores/loads
volatile ItuIfaceType g_itu_iface;

typedef ap_uint<70> AsuReqType;
typedef ap_uint<33> AsuRespType;
typedef ap_uint<32> AsuDataType;
typedef ap_uint<32> AsuAddrType;
typedef ap_uint<3> PredicateType;

unsigned findif (iterator begin, iterator end, PredicateType pred_val);

// ------------------------------------------------------------------
// processor interface
// ------------------------------------------------------------------
void top (volatile AsuReqType  &cfg_req,
          volatile AsuRespType &cfg_resp)
{
  static AsuDataType s_first_ds_id;
  static AsuDataType s_first_index;
  static AsuDataType s_last_ds_id;
  static AsuDataType s_last_index;
  static PredicateType s_pred;
  static unsigned s_result;

  AsuReqType req = cfg_req;
  AsuRespType resp;

  // handle write request
  if (((req >> 69) & 1) != 0) {
    AsuAddrType raddr = (req >> 64) & 0x1F;
    
    resp = 1;
    resp = resp << 32;

    // call the accelerator
    if (raddr == 0) {
      /*printf ("%u %u\n%u %u\n", (unsigned)s_first_ds_id, (unsigned)s_first_index, 
                                (unsigned)s_last_ds_id,  (unsigned)s_last_index);*/
      s_result = findif (
                   iterator(s_first_ds_id, s_first_index),
                   iterator(s_last_ds_id, s_last_index),
                   s_pred
                 );
    }
    else {
      AsuDataType data = (req >> 32) & 0xFFFF;
      // write internal regs
      switch (raddr) {
        case 1:
          s_first_ds_id = data; break;
        case 2:
          s_first_index = data; break;
        case 3:
          s_last_ds_id = data; break;
        case 4:
          s_last_index = data; break;
        case 5:
          s_pred = data; break;
        default:
          break;
      }
    }
  }
  // handle read request
  else {
    AsuAddrType raddr = (req >> 64) & 0x1F;
    resp = 0;

    switch (raddr) {
      case 0:
        resp |= s_result; break;
      case 1:
        resp |= s_first_ds_id; break;
      case 2:
        resp |= s_first_index; break;
      case 3:
        resp |= s_last_ds_id; break;
      case 4:
        resp |= s_last_index; break;
      case 5:
        resp |= s_pred; break;
      default:
        break;
    }
  }

  cfg_resp = resp;
}

// ------------------------------------------------------------------
// findif logic
// ------------------------------------------------------------------
unsigned findif (iterator begin, iterator end, PredicateType pred_val) {
//#pragma HLS INLINE
  bool stop = false;
  for (; begin != end; ++begin) {
    // 
    switch (pred_val) {
      case 0:
        if (*begin > 0) stop = true;
        break;
      case 1:
        if (*begin < 0) stop = true;
        break;
      case 2:
        if (*begin == 0) stop = true;
        break;
      case 3:
        if (*begin % 2 != 0) stop = true;
        break;
      case 4:
        if (*begin % 2 == 0) stop = true;
        break;
    };
    // exit loop if pred satisfied
    if (stop) {
      break;
    }
  }
  return begin.get_index();
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
AsuReqType make_req (AsuDataType data, AsuAddrType raddr, unsigned rw) {
  unsigned ds_id = 0;
  AsuReqType req = 0;
  req |= rw;      req = req << 5;
  req |= raddr;   req = req << 32;
  req |= data;    req = req << 32;
  req |= ds_id;
  return req;
}

void print_req (AsuReqType req) {
  //printf ("%s\n", req.to_string(16).c_str());
}
void print_resp (AsuRespType resp) {
  //printf ("%s\n", resp.to_string(16).c_str());
}

bool check_resp (AsuRespType resp) {
  resp = resp >> 32;
  if (resp != 1) return false;
  return true;
}

int main () {
  MyType myarray[N];
  for (int i = 0; i < N; ++i) myarray[i] = i-3;

  AsuRespType resp;
  AsuReqType req;
  AsuDataType data;
  AsuAddrType raddr;

  // set first ds id
  data = 0;   raddr = 1;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  print_resp (resp);
  assert( check_resp(resp) );
  
  // set first index
  data = 0;   raddr = 2;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set last ds id
  data = 0;   raddr = 3;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set last index
  data = 7;   raddr = 4;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  print_resp (resp);
  assert( check_resp(resp) );

  // set pred
  data = 4;   raddr = 5;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  print_resp (resp);
  assert( check_resp(resp) );

  // start accelerator
  data = 0;   raddr = 0;
  req = make_req( data, raddr, 1 );
  print_req (req);
  top( req, resp );
  check_resp( resp );
  print_resp (resp);
  assert( check_resp(resp) );

  // read result
  data = 0;   raddr = 0;
  req = make_req( data, raddr, 0 );
  print_req (req);
  top( req, resp );
  print_resp (resp);

  unsigned s = resp;
  printf ("--------------------\n");
  printf ("Result: %X\n", s);
  printf ("--------------------\n");
  //assert (s == 6);

  return 0;
}

