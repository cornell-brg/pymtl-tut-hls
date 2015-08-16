//========================================================================
// SortXcelHLS.cpp
//========================================================================
// C++ implementation for the SortXcel. Uses bubble sort. Accelerator
// register interface:
//
//  xr0 : go/done
//  xr1 : base address of array
//  xr2 : number of elements in array
//
// Accelerator protocol involves the following steps:
//  1. Write the base address of array via xr1
//  2. Write the number of elements in array via xr2
//  3. Tell accelerator to go by reading xr0 and wait till a result of 1 is
//  returned

#include "SortXcelHLS.h"

hls::stream<xcel::XcelReqMsg>  xcelreq;
hls::stream<xcel::XcelRespMsg> xcelresp;

using namespace xcel;
using namespace mem;

//------------------------------------------------------------------------
// sort
//------------------------------------------------------------------------

template < typename Array >
void sort( Array array )
{
#pragma HLS INLINE
  int n = array.size();
  for ( int i = 0; i < n; ++i ) {
    int prev = array[0];
    for ( int j = 1; j < n; ++j ) {
      #pragma HLS PIPELINE
      int curr = array[j];
      array[j-1] = std::min( prev, curr );
      prev       = std::max( prev, curr );
    }
    array[n-1] = prev;
  }
}

/*
template < typename Array >
void sort( Array array )
{
  int n = array.size();
  for ( int i = 0; i < n; ++i ) {

    bool is_sorted = true;
    for ( int j = 0; j < n-i-1; ++j ) {
      #pragma HLS pipeline
      int a = array[j];
      int b = array[j+1];
      if ( a > b ) {
        array[j]   = b;
        array[j+1] = a;
        is_sorted = false;
      }
    }

    if ( is_sorted )
      break;
  }
}
*/

//------------------------------------------------------------------------
// SortXcelHLS
//------------------------------------------------------------------------

#ifdef XILINX_VIVADO_HLS_TESTING
void SortXcelHLS
(
  mem::TestMem&             memreq,
  mem::TestMem&             memresp
){
#else
void SortXcelHLS
(
  hls::stream<MemReqMsg>&   memreq,
  hls::stream<MemRespMsg>&  memresp
){
#endif

  XcelWrapper<3> xcelWrapper;

  // configure
  xcelWrapper.configure();

  #ifdef XILINX_VIVADO_HLS_TESTING
  sort( ArrayMemPortAdapter<TestMem,TestMem> (
          memreq,
          memresp,
          xcelWrapper.get_xreg(1),
          xcelWrapper.get_xreg(2)
        ) );
  #else
  // compute
  sort( ArrayMemPortAdapter<hls::stream<MemReqMsg>, hls::stream<MemRespMsg> > (
          memreq,
          memresp,
          xcelWrapper.get_xreg(1),
          xcelWrapper.get_xreg(2)
        ) );
  #endif

  // signal done
  xcelWrapper.done( 1 );

}

