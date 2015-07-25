//========================================================================
// PolyDsuListHLS.cpp
//========================================================================
// Author  : Shreesha Srinath
// Date    : July 09, 2015
//
// C++ implementation of the list data-structure unit. The list
// data-structure unit has a data-structure request interface over which it
// receives a message sent by the dispatch unit to service a iterator-based
// request. The results of the computation are returned to the processor
// via the iterator response interface.
//
// NOTE: Currently the design handles only primitive data-types.
// TBD: Handle user-defined data-types.

#include <ap_utils.h>

#include "common.h"
#include "interfaces.h"

//------------------------------------------------------------------------
// Global constants
//------------------------------------------------------------------------

#define READ 0
#define WRITE 1
#define USER_DEFINED 11

//------------------------------------------------------------------------
// PolyDsuListHLS
//------------------------------------------------------------------------

void PolyDsuListHLS(
  hls::stream<IteratorReqMsg>&  xcelreq,
  hls::stream<IteratorRespMsg>& xcelresp,
  hls::stream<MemReqMsg>&       memreq,
  hls::stream<MemRespMsg>&      memresp,
  ap_uint<32>                   dtdesc
)
{

#pragma HLS PIPELINE II=1 enable_flush

  // Local variables
  MemRespMsg      mem_resp;

  if ( !xcelreq.empty() ) {

  IteratorReqMsg xcel_req = xcelreq.read();

  // get the dt_descriptor
  dtValue dt_desc = dtValue( dtdesc );

  // load request
  if      ( xcel_req.opc == 0 ) {
    memreq.write( MemReqMsg( 0, dt_desc.size, xcel_req.addr, 0, READ ) );
    ap_wait();
    mem_resp = memresp.read();
    xcelresp.write( IteratorRespMsg( mem_resp.data, xcel_req.addr, xcel_req.opc, xcel_req.ds_id, xcel_req.opq ) );
  }
  // store request
  else if ( xcel_req.opc == 1 ) {
    memreq.write( MemReqMsg( xcel_req.data, dt_desc.size, xcel_req.addr, 0, WRITE ) );
    ap_wait();
    mem_resp = memresp.read();
    xcelresp.write( IteratorRespMsg( 0, xcel_req.addr, xcel_req.opc, xcel_req.ds_id, xcel_req.opq ) );
  }
  // increment address
  else if ( xcel_req.opc == 2 ) {
    memreq.write( MemReqMsg( 0, 0, xcel_req.addr+8, 0, READ ) );
    ap_wait();
    mem_resp = memresp.read();
    xcelresp.write( IteratorRespMsg( 0, mem_resp.data, xcel_req.opc, xcel_req.ds_id, xcel_req.opq ) );
  }
  // decrement address
  else if ( xcel_req.opc == 3 ) {
    memreq.write( MemReqMsg( 0, 0, xcel_req.addr+4, 0, READ ) );
    ap_wait();
    mem_resp = memresp.read();
    xcelresp.write( IteratorRespMsg( 0, mem_resp.data, xcel_req.opc, xcel_req.ds_id, xcel_req.opq ) );
  }

 }

}


