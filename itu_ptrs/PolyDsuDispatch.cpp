//========================================================================
// PolyDsuDispatch.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : July 09, 2015
//
// C++ implementation of the data-structure dispatch unit. The dispatch
// unit receives a configuration or a iterator message from the processor
// unit and dispatches data-structure request to the appropriate
// data-structure unit.
//
// NOTE: Currently, the dispatch unit stores the dsu table and the metadata
// cache for primitive data-types. TBD: Handle user-defined data-types.

#include "PolyDsuDispatch.h"

//------------------------------------------------------------------------
// dsuTable::dsuTable
//------------------------------------------------------------------------

dsuTable::dsuTable()
{
#pragma HLS resource variable=table core=RAM_2P_1S
#pragma HLS resource variable=dtCache core=RAM_2P_1S
  for ( ap_uint<6> i = 0; i < noOfDsuEntries; ++i )
    this->table[i].valid = 0;
}

//------------------------------------------------------------------------
// dsu::allocate
//------------------------------------------------------------------------

ap_uint<11> dsuTable::allocate( ap_uint<4> dsType )
{
  // XXX: Need to handle the case when all entries are occupies
  for ( ap_uint<5> i = 0; i < noOfDsuEntries; ++i ) {
    if ( table[i].valid == 0 ) {
      table[i].ds_type = dsType;
      table[i].valid = 1;
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
  table[dsId].valid = 0;
}

//------------------------------------------------------------------------
// dsu::getDSType
//------------------------------------------------------------------------

ap_uint<4> dsuTable::getDSType( ap_uint<5> dsId )
{
  return table[dsId].ds_type;
}

//------------------------------------------------------------------------
// dsu::getDSDescriptor
//------------------------------------------------------------------------

ap_uint<32> dsuTable::getDSDescriptor( ap_uint<5> dsId )
{
  return table[dsId].ds_desc_ptr;
}

//------------------------------------------------------------------------
// dsu::getDTDescriptor
//------------------------------------------------------------------------

ap_uint<32> dsuTable::getDTDescriptor( ap_uint<5> dsId )
{
  // XXX: Currently, I am assuming that only primitive data-types exist. We
  // return the dtCache value
  return dtCache[dsId];
}

//------------------------------------------------------------------------
// dsu::setDSDescriptor
//------------------------------------------------------------------------

void dsuTable::setDSDescriptor( ap_uint<5> dsId, ap_uint<32> dsDescriptor )
{
  table[dsId].ds_desc_ptr = dsDescriptor;
}

//------------------------------------------------------------------------
// dsu::setDTDescriptor
//------------------------------------------------------------------------

void dsuTable::setDTDescriptor( ap_uint<5> dsId, ap_uint<32> dtDescriptor,
                                 hls::stream<MemReqMsg>&  memreq,
                                 hls::stream<MemRespMsg>& memresp )
{
  table[dsId].dt_desc_ptr = dtDescriptor;

  // XXX: Currently, I am assuming that only primitive data-types exist. We
  // load the dtCache at the configure phase for primitive data-types
  MemRespMsg mem_resp;

  // read the datatype descriptor
  memreq.write( MemReqMsg( 0, 0, dtDescriptor, 0, 0 ) );
  ap_wait();
  memresp.read( mem_resp );
  dtCache[dsId] = mem_resp.data;

}

//------------------------------------------------------------------------
// PolyDsuDispatch
//------------------------------------------------------------------------

void PolyDsuDispatch(
  hls::stream<XcelReqMsg>&     cfgreq,
  hls::stream<XcelRespMsg>&    cfgresp,
  hls::stream<IteratorReqMsg>& xcelreq,
  hls::stream<PolyDsuReqMsg>&  polydsureq,
  hls::stream<MemReqMsg>&      memreq,
  hls::stream<MemRespMsg>&     memresp
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
        ap_uint<11> ds_id = dsTable.allocate( cfg_req.data );
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
        dsTable.setDSDescriptor( ( cfg_req.id ), cfg_req.data );
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
      // Set DT descriptor
      else if ( cfg_req.addr == 2 ) {
        dsTable.setDTDescriptor( ( cfg_req.id ), cfg_req.data, memreq, memresp );
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
    }
  }
  // Check if an iterator request exists and dispatch work
  else if ( !xcelreq.empty() ) {
    IteratorReqMsg xcel_req = xcelreq.read();

    ap_uint<4>  dstype =  dsTable.getDSType( xcel_req.ds_id );
    ap_uint<32> dtdesc = dsTable.getDTDescriptor( xcel_req.ds_id );

    polydsureq.write(
      PolyDsuReqMsg(
        dstype,
        dtdesc,
        xcel_req.data,
        xcel_req.addr,
        xcel_req.iter,
        xcel_req.opc,
        xcel_req.ds_id,
        xcel_req.opq
     )
   );

  }

}


