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
    table[i].valid = 0;
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
#define USER_DEFINED 14
#define VECTOR 0
#define LIST 1

void IteratorTranslationUnitHLS(
  hls::stream<XcelReqMsg>&      cfgreq,
  hls::stream<XcelRespMsg>&     cfgresp,
  hls::stream<IteratorReqMsg>&  xcelreq,
  hls::stream<IteratorRespMsg>& xcelresp,
  hls::stream<MemReqMsg>&       memreq,
  hls::stream<MemRespMsg>&      memresp,
  hls::stream<ap_uint<32> >     debug
)
{

  // Local variables
  static XcelReqMsg  cfg_req;
  XcelRespMsg     cfg_resp;

  MemRespMsg      mem_resp;

  static IteratorReqMsg  xcel_req;

  // create a dstuTable
  static dstuTable dsTable;

  static enum {IDLE, CONFIGURE, TRANSLATE} dstuState = IDLE;

  switch ( dstuState ) {
    case IDLE:
    {
      if ( !cfgreq.empty() ) {
        cfgreq.read( cfg_req );
        dstuState = CONFIGURE;
      }
      else if ( !xcelreq.empty() ) {
        xcelreq.read( xcel_req );
        dstuState = TRANSLATE;
      }
      break;
    }

    case CONFIGURE: {

      if ( cfg_req.id == DSTU_ID ) {
        if ( cfg_req.addr == 1 ) {
          cfg_resp = XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq );
          cfg_resp.data = dsTable.allocate( cfg_req.data );
          cfgresp.write( cfg_resp );
        }
        else if ( cfg_req.addr == 2 ) {
          cfg_resp = XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq );
          dsTable.deallocate( cfg_req.data );
          cfgresp.write( cfg_resp );
        }
      }
      else if ( cfg_req.id >= 0x400 ) {
        if ( cfg_req.addr == 1 ) {
          cfg_resp = XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq );
          dsTable.setDSDescriptor( ( cfg_req.id ), cfg_req.data );
          cfgresp.write( cfg_resp );
        }
        else if ( cfg_req.addr == 2 ) {
          cfg_resp = XcelRespMsg( cfg_req.id, 0, cfg_req.type, cfg_req.opq );
          dsTable.setDTDescriptor( ( cfg_req.id ), cfg_req.data );
          cfgresp.write( cfg_resp );
        }
      }

      dstuState = IDLE;
      break;
    }

    case TRANSLATE:
    {
      // get the ds_type
      ap_uint<4> dsType = dsTable.getDSType( xcel_req.ds_id );

      if ( dsType == VECTOR ) {
        // get the ds_desc_ptr
        ap_uint<32> base   = dsTable.getDSDescriptor( xcel_req.ds_id );
        // get the dt_desc_ptr
        ap_uint<32> dt_ptr = dsTable.getDTDescriptor( xcel_req.ds_id );

        // read the dt_descriptor
        memreq.write( MemReqMsg( 0, 0, dt_ptr, 0, READ ) );
        ap_wait();
        memresp.read( mem_resp );
        ap_wait();

        dtValue dt_desc = dtValue( mem_resp.data );
        debug.write( mem_resp.data );

        // handle primitive data types
        if ( dt_desc.type < USER_DEFINED ) {
          ap_uint<32> mem_addr = base + xcel_req.iter * dt_desc.size;
          if ( xcel_req.type == READ  ) {
            memreq.write( MemReqMsg( 0, dt_desc.size, mem_addr, 0, READ ) );
            ap_wait();
            memresp.read( mem_resp );
            xcelresp.write( IteratorRespMsg( xcel_req.ds_id, mem_resp.data, xcel_req.type, xcel_req.opq ) );
          }
          else if ( xcel_req.type == WRITE ) {
            memreq.write( MemReqMsg( xcel_req.data, dt_desc.size, mem_addr, 0, WRITE ) );
            ap_wait();
            memresp.read( mem_resp );
            xcelresp.write( IteratorRespMsg( xcel_req.ds_id, 0, xcel_req.type, xcel_req.opq ) );
          }
        }
      }
      else if ( dsType == LIST ) {
        ap_uint<32> node_ptr = dsTable.getDSDescriptor( xcel_req.ds_id );
        // get the dt_desc_ptr
        ap_uint<32> dt_ptr = dsTable.getDTDescriptor( xcel_req.ds_id );

        // read the dt_descriptor
        memreq.write( MemReqMsg( 0, 0, dt_ptr, 0, READ ) );
        ap_wait();
        memresp.read( mem_resp );
        ap_wait();

        dtValue dt_desc = dtValue( mem_resp.data );
        debug.write( mem_resp.data );

        // handle primitive data types
        if ( dt_desc.type < USER_DEFINED ) {
          if ( xcel_req.iter > 0 ) {
            for ( int i = 0; i < xcel_req.iter; ++ i ) {
              memreq.write( MemReqMsg( 0, 0, node_ptr + 8, 0, READ ) );
              ap_wait();
              memresp.read( mem_resp );
              node_ptr = mem_resp.data;
            }
          }

          if ( xcel_req.type == READ  ) {
            memreq.write( MemReqMsg( 0, dt_desc.size, node_ptr, 0, READ ) );
            ap_wait();
            memresp.read( mem_resp );
            xcelresp.write( IteratorRespMsg( xcel_req.ds_id, mem_resp.data, xcel_req.type, xcel_req.opq ) );
          }
          else if ( xcel_req.type == WRITE ) {
            memreq.write( MemReqMsg( xcel_req.data, dt_desc.size, node_ptr, 0, WRITE ) );
            ap_wait();
            memresp.read( mem_resp );
            xcelresp.write( IteratorRespMsg( xcel_req.ds_id, 0, xcel_req.type, xcel_req.opq ) );
          }
        }
      }
      dstuState = IDLE;
      break;
    }
  }

}


