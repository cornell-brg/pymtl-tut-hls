#include <assert.h>
#include <stdio.h>
#define N 10

#include "../common/common.h"

typedef char MyType;
typedef _iterator<MyType> iterator;

ItuIfaceType g_itu_iface;

typedef ap_uint<70> AsuReqType;
typedef ap_uint<33> AsuRespType;
typedef ap_uint<32> AsuDataType;
typedef ap_uint<32> AsuAddrType;
typedef ap_uint<3> PredicateType;

unsigned findif (iterator begin, iterator end, PredicateType pred_val);

// ------------------------------------------------------------------
// processor interface
// ------------------------------------------------------------------
void top (AsuReqType cfg_req, AsuRespType &cfg_resp) {
  static AsuDataType s_first_ds_id;
  static AsuDataType s_first_index;
  static AsuDataType s_last_ds_id;
  static AsuDataType s_last_index;
  static PredicateType s_pred;

  AsuReqType req = cfg_req;
  AsuRespType resp;

  // handle write request
  if (((req >> 69) & 1) != 0) {
    AsuDataType data = (req >> 32) & 0xFFFF;
    AsuAddrType raddr = (req >> 64) & 0x1F;
    
    resp = 1;
    resp = resp << 32;
    
    // call the accelerator
    if (raddr == 0) {
      /*printf ("%u %u\n%u %u\n", (unsigned)s_first_ds_id, (unsigned)s_first_index, 
                                (unsigned)s_last_ds_id,  (unsigned)s_last_index);*/
      iterator begin (s_first_ds_id, s_first_index);
      iterator end   (s_last_ds_id,  s_last_index);
      unsigned resp_data = findif (begin, end, s_pred);
      
      resp |= resp_data;
    }

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
  // handle read request
  else {
    resp = 0;
  }

  cfg_resp = resp;
}

// ------------------------------------------------------------------
// findif logic
// ------------------------------------------------------------------
unsigned findif (iterator begin, iterator end, PredicateType pred_val) {
  int temp = *begin;
  *begin = temp+1;
  return 0;
}

// ------------------------------------------------------------------
// helpers for main
// ------------------------------------------------------------------
AsuReqType make_req (AsuDataType data, AsuAddrType raddr, unsigned rw) {
  unsigned ds_id = 1;
  AsuReqType req = 0;
  req |= rw;      req = req << 5;
  req |= raddr;   req = req << 32;
  req |= data;    req = req << 32;
  req |= ds_id;
  return req;
}

void print_req (AsuReqType req) {
  for (int i = 70-1; i >= 0; --i) {
    printf ("%c", req[i] ? '1' : '0');
  }
  printf ("\n");
}
void print_resp (AsuRespType resp) {
  for (int i = 33-1; i >= 0; --i) {
    printf ("%c", resp[i] ? '1' : '0');
  }
  printf ("\n");
}

bool check_resp (AsuRespType resp) {
  resp = resp >> 32;
  if (resp != 1) return false;
  return true;
}

int main () {
  MyType myarray[N];
  for (int i = 0; i < N; ++i) myarray[i] = i-3;

  iterator begin (0, 0);
  iterator end   (0, N);

  AsuRespType resp;
  AsuReqType req;
  AsuDataType data;
  AsuAddrType raddr;

  // set first ds id
  data = 1;   raddr = 1;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );
  
  // set first index
  data = 0;   raddr = 2;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );

  // set last ds id
  data = 1;   raddr = 3;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );

  // set last index
  data = N;   raddr = 4;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );

  // set pred
  data = 0;   raddr = 5;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );

  // start accelerator
  data = 0;   raddr = 0;
  req = make_req( data, raddr, 1 );
  top( req, resp );
  assert( check_resp(resp) );

  unsigned s = resp;
  printf ("--------------------\n");
  printf ("Result: %u\n", s);
  printf ("--------------------\n");
  assert (s == 0);

  return 0;
}

