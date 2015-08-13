//========================================================================
// MemProxyHLS
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// HLS tests for MemProxy. This is not really meant to be used as real
// hardware but is instead used to test that HLS works with MemProxy. We
// use a very simple accelerator register interface. The test harness
// simply writes xr0 with the desired test to run. The accelerator only
// returns the response message when the test is finished.


#include "MemStream.h"
#include "MemProxy.h"
#include "XcelMsg.h"

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  TestMem MemProxyHLS_mem;
  TestMem& memreq  = MemProxyHLS_mem;
  TestMem& memresp = MemProxyHLS_mem;
#else
  hls::stream<MemReqMsg>  memreq;
  hls::stream<MemRespMsg> memresp;
#endif

//------------------------------------------------------------------------
// test_basic_write
//------------------------------------------------------------------------

void test_basic_write()
{
  mem::MemProxy<int> a(0x1000);
  a = 42;
}

//------------------------------------------------------------------------
// test_copy
//------------------------------------------------------------------------

void test_copy()
{
  mem::MemProxy<int> a(0x1000);
  mem::MemProxy<int> b(0x2000);
  b = a;
}

//------------------------------------------------------------------------
// test_operators
//------------------------------------------------------------------------

void test_operators()
{
  mem::MemProxy<int> a(0x2000);
  mem::MemProxy<int> b(0x2004);

  a = 42;
  b = 47;

  bool eq  = ( a == b );
  bool neq = ( a != b );
  bool lt  = ( a <  b );
  bool gt  = ( a >  b );

  // Save values so we can check them (requires three writes)

  mem::OutMemStream os(0x2008);
  os << eq << neq << lt << gt;
}

//------------------------------------------------------------------------
// test_memoize
//------------------------------------------------------------------------

void test_memoize()
{
  mem::MemProxy<int> a(0x1000);
  int b = a;
  int c = a;

  // Save values so we can check them (requires three writes)

  mem::OutMemStream os(0x2000);
  os << a << b << c;
}

//------------------------------------------------------------------------
// MemProxyHLS
//------------------------------------------------------------------------

using namespace xcel;

void MemProxyHLS
(
  hls::stream<XcelReqMsg>&  xcelreq,
  hls::stream<XcelRespMsg>& xcelresp
){
#pragma HLS INLINE

  XcelReqMsg req = xcelreq.read();
  int test_num = req.data;

  switch ( test_num ) {
    case 0: test_basic_write(); break;
    case 1: test_copy();        break;
    case 2: test_operators();   break;
    case 3: test_memoize();     break;
  }

  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );
}

