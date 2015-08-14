//========================================================================
// SortXcelHLS.cpp
//========================================================================
// Author  : Christopher Batten
// Date    : July 2, 2015
//
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
//  3. Tell accelerator to go by writing xr0
//  4. Wait for accelerator to finish by reading xr0, result will be 1

#include "SortXcelHLS.h"
#include <ap_int.h>
#include <ap_utils.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  mem::TestMem SortXcelHLS_mem;
  mem::TestMem& memreq  = SortXcelHLS_mem;
  mem::TestMem& memresp = SortXcelHLS_mem;
#else
  hls::stream<mem::MemReqMsg>  memreq;
  hls::stream<mem::MemRespMsg> memresp;
#endif

using namespace xcel;
using namespace mem;

//------------------------------------------------------------------------
// ArrayMemPortAdapter
//------------------------------------------------------------------------
// This class provides a standard array interface, but the implementation
// essentially turns reads/writes into memory requests sent over an HLS
// stream interface. This is analogous to the pclib.fl.ListMemPortAdapter
// in PyMTL ... except this one is synthesizable into RTL!

class ArrayMemPortAdapter {

 public:

  class ArrayMemPortAdapterProxy {

   public:

    ArrayMemPortAdapterProxy( int addr )
     : m_addr(addr)
    {
    }

    // Read

    int read() const
    {
      // memory read request

      memreq.write( MemReqMsg( 0, 0, m_addr, 0, MemReqMsg::TYPE_READ ) );
      ap_wait();

      // memory read response

      MemRespMsg resp = memresp.read();

      // Return the data

      return static_cast<int>(resp.data);
    }

    // Write

    void write( int value )
    {
      // memory write request

      memreq.write( MemReqMsg( value, 0, m_addr, 0, MemReqMsg::TYPE_WRITE ) );
      ap_wait();

      // memory write response

      MemRespMsg resp = memresp.read();
    }

    // cast

    operator int() const
    {
      return read();
    }

    // lvalue use of the proxy object

    ArrayMemPortAdapterProxy& operator=( int value )
    {
      write( value );
      return *this;
    }

    // lvalue use of the proxy object with proxy on RHS

    ArrayMemPortAdapterProxy& operator=( const ArrayMemPortAdapterProxy& rhs )
    {
      int value = rhs.read();
      write( value );
      return *this;
    }

   private:

    int m_addr;

  };

  ArrayMemPortAdapter( int base, int size )
   : m_base(base), m_size(size)
  {
  }

  // Size

  int size() const
  {
    return m_size;
  }

  // Write

  ArrayMemPortAdapterProxy operator[]( int i )
  {
    return ArrayMemPortAdapterProxy( m_base + i*4 );
  }

  // Read

  int operator[]( int i ) const
  {
    ArrayMemPortAdapterProxy proxy( m_base + i*4 );
    return proxy.read();
  }

 private:

  int m_base;
  int m_size;

};

//------------------------------------------------------------------------
// sort
//------------------------------------------------------------------------

template < typename Array >
void sort( Array array )
{
  int n = array.size();
  for ( int i = 0; i < n; ++i ) {
    int prev = array[0];
    for ( int j = 1; j < n; ++j ) {
      #pragma HLS pipeline
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

void SortXcelHLS
(
  hls::stream<XcelReqMsg>&  xcelreq,
  hls::stream<XcelRespMsg>& xcelresp
){
#pragma HLS INLINE

  // Local variables
  XcelReqMsg  req;

  // 1. Write the base address of the array to xr1
  req = xcelreq.read();
  ap_uint<32> base = req.data;
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  // 2. Write the number of elements in the array to xr2
  req = xcelreq.read();
  ap_uint<32> size = req.data;
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  // Wait till the "go" request is received
  while ( xcelreq.empty() ) {}

  // 3. Tell accelerator to go by writing xr0
  req = xcelreq.read();
  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );

  // Compute
  sort( ArrayMemPortAdapter( base, size ) );

  // 4. Wait for accelerator to finish by reading xr0, result will be 1
  req = xcelreq.read();
  xcelresp.write( XcelRespMsg( req.id, 1, req.type, req.opq ) );
}

