//========================================================================
// PolyDsuConfigHLS.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : July 09, 2015
//
//
// C++ implementation of the data-structure unit configuration logic.
// The unit receives a configuration message and sets state for the
// data-structure unit. The state that stores data-structure/
// data-type descriptors and the data-structre type.
//
// NOTE: The design currently supports only primitive data-types.

#include "PolyDsuConfigHLS.h"

//------------------------------------------------------------------------
// dsuTable::dsuTable
//------------------------------------------------------------------------

dsuTable::dsuTable()
{
#pragma HLS resource variable=table core=RAM_2P_1S
  for ( ap_uint<6> i = 0; i < noOfDsuEntries; ++i )
    table[i] = 0;
}

//------------------------------------------------------------------------
// dsu::allocate
//------------------------------------------------------------------------

ap_uint<11> dsuTable::allocate( ap_uint<4> dsType, ap_uint<4> dstype[32] )
{
  // XXX: Need to handle the case when all entries are occupies
  for ( ap_uint<5> i = 0; i < noOfDsuEntries; ++i ) {
    if ( table[i] == 0 ) {
      table[i]  = 1;
      dstype[i] = dsType;
      return ( 1024 + i );
    }
  }
  return 0;
}

//------------------------------------------------------------------------
// dsu::deallocate
//------------------------------------------------------------------------

void dsuTable::deallocate( ap_uint<5> dsId )
{
  table[dsId] = 0;
}

//------------------------------------------------------------------------
// PolyDsuConfigHLS
//------------------------------------------------------------------------

void PolyDsuConfigHLS(
  hls::stream<XcelReqMsg>&     cfgreq,
  hls::stream<XcelRespMsg>&    cfgresp,
  hls::stream<MemReqMsg>&      memreq,
  hls::stream<MemRespMsg>&     memresp,
  ap_uint<4>                   dstype[32],
  ap_uint<32>                  dsdesc[32],
  ap_uint<32>                  dtdesc[32]
)
{

  // Local variables
  XcelReqMsg  cfg_req;
  MemRespMsg  mem_resp;

  // Create a dsuTable
  static dsuTable dsTable;

  // Check if a configuration request exists and process it
  if ( !cfgreq.empty() ) {
    cfgreq.read( cfg_req );

    // Process DTSU requests
    if ( cfg_req.id == DSTU_ID ) {
      // Allocate
      if ( cfg_req.addr == 1 ) {
        ap_uint<11> ds_id = dsTable.allocate( cfg_req.data, dstype );
        cfgresp.write( XcelRespMsg( cfg_req.id, ds_id, cfg_req.type, cfg_req.opq ) );
      }
      // Deallocte
      else if ( cfg_req.addr == 2 ) {
        dsTable.deallocate( cfg_req.data );
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
    }
    // Process DSU requests
    else if ( cfg_req.id >= 0x400 ) {
      // Set DS descriptor
      if ( cfg_req.addr == 1 ) {
        dsdesc[cfg_req.id] = cfg_req.data;
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
      // Set DT descriptor
      else if ( cfg_req.addr == 2 ) {
        // XXX: Currently, I am assuming that only primitive data-types exist. We
        // load the dtCache at the configure phase for primitive data-types
        MemRespMsg mem_resp;

        // read the datatype descriptor
        memreq.write( MemReqMsg( 0, 0, cfg_req.data, 0, 0 ) );
        ap_wait();
        memresp.read( mem_resp );
        dtdesc[cfg_req.id] = mem_resp.data;
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
    }
  }
}
