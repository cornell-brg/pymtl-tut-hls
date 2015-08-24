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

#include "mem/MemStream.h"
#include "mem/MemProxy.h"
#include "xcel/XcelMsg.h"

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

//------------------------------------------------------------------------
// test_basic_write
//------------------------------------------------------------------------

void test_basic_write( mem::MemReqStream& memreq, mem::MemRespStream& memresp )
{
  mem::MemValue<int> a(0x1000,memreq,memresp);
  a = 42;
}

//------------------------------------------------------------------------
// test_copy
//------------------------------------------------------------------------

void test_copy( mem::MemReqStream& memreq, mem::MemRespStream& memresp )
{
  mem::MemValue<int> a(0x1000,memreq,memresp);
  mem::MemValue<int> b(0x2000,memreq,memresp);
  b = a;
}

//------------------------------------------------------------------------
// test_operators
//------------------------------------------------------------------------

void test_operators( mem::MemReqStream& memreq, mem::MemRespStream& memresp )
{
  mem::MemValue<int> a(0x2000,memreq,memresp);
  mem::MemValue<int> b(0x2004,memreq,memresp);

  a = 42;
  b = 47;

  bool eq  = ( a == b );
  bool neq = ( a != b );
  bool lt  = ( a <  b );
  bool gt  = ( a >  b );

  // Save values so we can check them (requires three writes)

  mem::OutMemStream os(0x2008,memreq,memresp);
  os << eq << neq << lt << gt;
}

//------------------------------------------------------------------------
// test_memoize
//------------------------------------------------------------------------

void test_memoize( mem::MemReqStream& memreq, mem::MemRespStream& memresp )
{
  mem::MemValue<int> a(0x1000,memreq,memresp);
  int b = a;
  int c = a;

  // Save values so we can check them (requires three writes)

  mem::OutMemStream os(0x2000,memreq,memresp);
  os << a << b << c;
}

//------------------------------------------------------------------------
// MemProxyHLS
//------------------------------------------------------------------------

using namespace xcel;

void MemProxyHLS
(
  hls::stream<XcelReqMsg>&  xcelreq,
  hls::stream<XcelRespMsg>& xcelresp,
  mem::MemReqStream&        memreq,
  mem::MemRespStream&       memresp
){
  XcelReqMsg req = xcelreq.read();
  int test_num = req.data();

  switch ( test_num ) {
    case 0: test_basic_write(memreq,memresp); break;
    case 1: test_copy(memreq,memresp);        break;
    case 2: test_operators(memreq,memresp);   break;
    case 3: test_memoize(memreq,memresp);     break;
  }

  xcelresp.write( XcelRespMsg( req.opq(), req.type(), 0, req.id() ) );
}

