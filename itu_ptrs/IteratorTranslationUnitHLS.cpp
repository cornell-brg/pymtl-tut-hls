//========================================================================
// IteratorTranslationUnitHLS.cpp
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 12, 2015
//
// C++ implementation of the data-structure translation unit.
//

#include "IteratorTranslationUnitHLS.h"

//------------------------------------------------------------------------
// dstuTable::dstuTable
//------------------------------------------------------------------------

dstuTable::dstuTable()
{
  for ( ap_uint<6> i = 0; i < noOfDstuEntries; ++i )
    this->table[i].valid = 0;
}

//------------------------------------------------------------------------
// dstu::allocate
//------------------------------------------------------------------------

ap_uint<11> dstuTable::allocate( ap_uint<4> dsType )
{
  // XXX: Need to handle the case when all entries are occupies
  for ( ap_uint<5> i = 0; i < noOfDstuEntries; ++i ) {
    if ( this->table[i].valid == 0 ) {
      this->table[i].ds_type = dsType;
      this->table[i].valid = 1;
      return ( 1024 + i );
    }
  }
  return 0;
}

//------------------------------------------------------------------------
// dstu::deallocate
//------------------------------------------------------------------------

void dstuTable::deallocate( ap_uint<5> dsId )
{
  this->table[dsId].valid = 0;
}

//------------------------------------------------------------------------
// dstu::getDSType
//------------------------------------------------------------------------

ap_uint<4> dstuTable::getDSType( ap_uint<5> dsId )
{
  return this->table[dsId].ds_type;
}

//------------------------------------------------------------------------
// dstu::getDSDescriptor
//------------------------------------------------------------------------

ap_uint<32> dstuTable::getDSDescriptor( ap_uint<5> dsId )
{
  return this->table[dsId].ds_desc_ptr;
}

//------------------------------------------------------------------------
// dstu::getDTDescriptor
//------------------------------------------------------------------------

ap_uint<32> dstuTable::getDTDescriptor( ap_uint<5> dsId )
{
  return this->table[dsId].dt_desc_ptr;
}

//------------------------------------------------------------------------
// dstu::setDSDescriptor
//------------------------------------------------------------------------

void dstuTable::setDSDescriptor( ap_uint<5> dsId, ap_uint<32> dsDescriptor )
{
  this->table[dsId].ds_desc_ptr = dsDescriptor;
}

//------------------------------------------------------------------------
// dstu::setDTDescriptor
//------------------------------------------------------------------------

void dstuTable::setDTDescriptor( ap_uint<5> dsId, ap_uint<32> dtDescriptor )
{
  this->table[dsId].dt_desc_ptr = dtDescriptor;
}

//------------------------------------------------------------------------
// IteratorTranslationUnitHLS
//------------------------------------------------------------------------

#define DSTU_ID 3
#define READ 0
#define WRITE 1
#define USER_DEFINED 11
#define VECTOR 0
#define LIST 1

void IteratorTranslationUnitHLS(
  hls::stream<XcelReqMsg>&      cfgreq,
  hls::stream<XcelRespMsg>&     cfgresp,
  hls::stream<IteratorReqMsg>&  xcelreq,
  hls::stream<IteratorRespMsg>& xcelresp,
  hls::stream<MemReqMsg>&       memreq,
  hls::stream<MemRespMsg>&      memresp
  //hls::stream<ap_uint<32> >     debug
)
{

  // Local variables
  XcelReqMsg      cfg_req;
  IteratorReqMsg  xcel_req;
  MemRespMsg      mem_resp;

  // Create a dstuTable
  static dstuTable dsTable;

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
        dsTable.setDTDescriptor( ( cfg_req.id ), cfg_req.data );
        cfgresp.write( XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq ) );
      }
    }
  }
  // Check if an iterator request exists and process it
  else if ( !xcelreq.empty() ) {
    xcelreq.read( xcel_req );

    // get the ds_type
    ap_uint<4> dsType = dsTable.getDSType( xcel_req.ds_id );

    if ( dsType == LIST ) {
      // get the dt_desc_ptr
      ap_uint<32> dt_ptr = dsTable.getDTDescriptor( xcel_req.ds_id );

      // read the datatype descriptor
      memreq.write( MemReqMsg( 0, 0, dt_ptr, 0, READ ) );
      ap_wait();
      memresp.read( mem_resp );

      dtValue dt_desc = dtValue( mem_resp.data );

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

}


