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

#ifndef POLYDSU_DISPATCH_H
#define POLYDSU_DISPATCH_H

#include <ap_utils.h>

#include "common.h"
#include "interfaces.h"

//------------------------------------------------------------------------
// Global constants
//------------------------------------------------------------------------

#define DSTU_ID 3
#define READ 0
#define WRITE 1
#define USER_DEFINED 11
#define noOfDsuEntries 32

//------------------------------------------------------------------------
// dsuTableEntry
//------------------------------------------------------------------------

struct dsuTableEntry{
  ap_uint<32> dt_desc_ptr;
  ap_uint<32> ds_desc_ptr;
  ap_uint<4>  ds_type;
  ap_uint<1>  valid;

  dsuTableEntry() : dt_desc_ptr(0), ds_desc_ptr(0),
                     ds_type(0), valid(0) {}

  dsuTableEntry( ap_uint<32> dtDescriptor, ap_uint<32> dsDescriptor,
                  ap_uint<4> dsType, ap_uint<1> valid_ )
    : dt_desc_ptr( dtDescriptor ), ds_desc_ptr( dsDescriptor ),
      ds_type( dsType ), valid( valid_ ) {}

};

//------------------------------------------------------------------------
// dsuTable
//------------------------------------------------------------------------

class dsuTable {
  public:
    dsuTableEntry table[noOfDsuEntries];
    unsigned int   dtCache[noOfDsuEntries];

    dsuTable();

    // allocates an entry in the dsuTable and returns the index of the
    // entry allocated for the requested data-structure type
    ap_uint<11> allocate( ap_uint<4> dsType );

    // de-allocates an entry in the dsuTable given the ds-id
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
// PolyDsuDispatch
//------------------------------------------------------------------------

void PolyDsuDispatch(
  hls::stream<XcelReqMsg>&     cfgreq,
  hls::stream<XcelRespMsg>&    cfgresp,
  hls::stream<IteratorReqMsg>& xcelreq,
  hls::stream<PolyDsuReqMsg>&  polydsureq,
  hls::stream<MemReqMsg>&      memreq,
  hls::stream<MemRespMsg>&     memresp
);

#endif

