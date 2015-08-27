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

#include "xmem/MemProxyHLS.h"

//------------------------------------------------------------------------
// test_basic_write
//------------------------------------------------------------------------

void test_basic_write( ap_uint<8> opq, xmem::MemReqStream& memreq, xmem::MemRespStream& memresp )
{
  xmem::MemValue<int> a(0x1000,opq,memreq,memresp);
  a = 42;
}

//------------------------------------------------------------------------
// test_copy
//------------------------------------------------------------------------

void test_copy( ap_uint<8> opq, xmem::MemReqStream& memreq, xmem::MemRespStream& memresp )
{
  xmem::MemValue<int> a(0x1000,opq,memreq,memresp);
  xmem::MemValue<int> b(0x2000,opq,memreq,memresp);
  b = a;
}

//------------------------------------------------------------------------
// test_operators
//------------------------------------------------------------------------

void test_operators( ap_uint<8> opq, xmem::MemReqStream& memreq, xmem::MemRespStream& memresp )
{
  xmem::MemValue<int> a(0x2000,opq,memreq,memresp);
  xmem::MemValue<int> b(0x2004,opq,memreq,memresp);

  a = 42;
  b = 47;

  bool eq  = ( a == b );
  bool neq = ( a != b );
  bool lt  = ( a <  b );
  bool gt  = ( a >  b );

  // Save values so we can check them (requires three writes)

  xmem::OutMemStream os(0x2008,opq,memreq,memresp);
  os << eq << neq << lt << gt;
}

//------------------------------------------------------------------------
// test_memoize
//------------------------------------------------------------------------

void test_memoize( ap_uint<8> opq, xmem::MemReqStream& memreq, xmem::MemRespStream& memresp )
{
  xmem::MemValue<int> a(0x1000,opq,memreq,memresp);
  int b = a;
  int c = a;

  // Save values so we can check them (requires three writes)

  xmem::OutMemStream os(0x2000,opq,memreq,memresp);
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
  xmem::MemReqStream&       memreq,
  xmem::MemRespStream&      memresp
){
  XcelReqMsg req = xcelreq.read();
  int test_num = req.data();

  switch ( test_num ) {
    case 0: test_basic_write(req.opq(),memreq,memresp); break;
    case 1: test_copy(req.opq(),memreq,memresp);        break;
    case 2: test_operators(req.opq(),memreq,memresp);   break;
    case 3: test_memoize(req.opq(),memreq,memresp);     break;
  }

  xcelresp.write( XcelRespMsg( req.opq(), req.type(), 0, req.id() ) );
}

