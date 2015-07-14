//========================================================================
// PolyDsuConfig.h
//========================================================================
// Author  : Shreesha Srinath
// Date    : July 09, 2015
//
// C++ implementation of the data-structure unit configuration logic.
// The unit receives a configuration message and sets state for the
// data-structure unit. The state that stores data-structure/
// data-type descriptors and the data-structre type.
//
// NOTE: The design currently supports only primitive data-types.

#ifndef POLYDSU_CONFIG_H
#define POLYDSU_CONFIG_H

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
    ap_uint<1>    table[noOfDsuEntries];

    dsuTable();

    // allocates an entry in the dsuTable and returns the index of the
    // entry allocated for the requested data-structure type
    ap_uint<11> allocate( ap_uint<4> dsType, ap_uint<4> dstype[32] );

    // de-allocates an entry in the dsuTable given the ds-id
    void deallocate( ap_uint<5> dsId );
};

//------------------------------------------------------------------------
// PolyDsuConfig
//------------------------------------------------------------------------

void PolyDsuConfig(
  hls::stream<XcelReqMsg>&     cfgreq,
  hls::stream<XcelRespMsg>&    cfgresp,
  hls::stream<MemReqMsg>&      memreq,
  hls::stream<MemRespMsg>&     memresp,
  ap_uint<4>  dstype[32],
  ap_uint<32> dtdesc[32]
);

#endif

