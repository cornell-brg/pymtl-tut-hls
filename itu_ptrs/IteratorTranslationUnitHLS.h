//========================================================================
// dstu.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : June 12, 2015
//
// C++ implementation of the data-structure translation unit.
//

#ifndef ITERATOR_TRANSLATION_UNIT_HLS_H
#define ITERATOR_TRANSLATION_UNIT_HLS_H

#include "common.h"
#include "interfaces.h"
#include <ap_utils.h>

#define noOfDstuEntries 32

//------------------------------------------------------------------------
// dstuTableEntry
//------------------------------------------------------------------------

struct dstuTableEntry{
  ap_uint<32> dt_desc_ptr;
  ap_uint<32> ds_desc_ptr;
  ap_uint<4>  ds_type;
  ap_uint<1>  valid;

  dstuTableEntry() : dt_desc_ptr(0), ds_desc_ptr(0),
                     ds_type(0), valid(0) {}

  dstuTableEntry( ap_uint<32> dtDescriptor, ap_uint<32> dsDescriptor,
                  ap_uint<4> dsType, ap_uint<1> valid_ )
    : dt_desc_ptr( dtDescriptor ), ds_desc_ptr( dsDescriptor ),
      ds_type( dsType ), valid( valid_ ) {}

};

//------------------------------------------------------------------------
// dstuTable
//------------------------------------------------------------------------

class dstuTable {
  public:
    dstuTableEntry table[noOfDstuEntries];
    unsigned int   dtCache[noOfDstuEntries];

    dstuTable();

    // allocates an entry in the dstuTable and returns the index of the
    // entry allocated for the requested data-structure type
    ap_uint<11> allocate( ap_uint<4> dsType );

    // de-allocates an entry in the dstuTable given the ds-id
    void deallocate( ap_uint<5> dsId );

    // gets the ds_type given an index
    ap_uint<4> getDSType( ap_uint<5> dsId );

    // gets the ds_descriptor given an index
    ap_uint<32> getDSDescriptor( ap_uint<5> dsId );

    // gets the dt_descriptor given an index
    ap_uint<32> getDTDescriptor( ap_uint<5> dsId );

    // sets the ds_descriptor given an index
    void setDSDescriptor( ap_uint<5> dsId, ap_uint<32> dsDescriptor );

    // sets the dt_descriptor given an index
    void setDTDescriptor( ap_uint<5> dsId, ap_uint<32> dtDescriptor,
                          hls::stream<MemReqMsg>&  memreq,
                          hls::stream<MemRespMsg>& memresp );

};

//------------------------------------------------------------------------
// IteratorTranslationUnitHLS
//------------------------------------------------------------------------

void IteratorTranslationUnitHLS(
  hls::stream<XcelReqMsg>&      cfgreq,
  hls::stream<XcelRespMsg>&     cfgresp,
  hls::stream<IteratorReqMsg>&  xcelreq,
  hls::stream<IteratorRespMsg>& xcelresp,
  hls::stream<MemReqMsg>&       memreq,
  hls::stream<MemRespMsg>&      memresp,
  hls::stream<ap_uint<32> >     debug
);

#endif

