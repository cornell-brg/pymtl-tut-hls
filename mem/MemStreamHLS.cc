//========================================================================
// MemStreamHLS
//========================================================================
// Author  : Christopher Batten
// Date    : August 5, 2015
//
// HLS tests for MemStream. This is not really meant to be used as real
// hardware but is instead used to test that HLS works with MemStreams.
// We use a very simple accelerator register interface. The test harness
// simply writes xr0 with the desired test to run. The accelerator only
// returns the response message when the test is finished.

#include "MemStream.h"
#include "../xcel/XcelMsg.h"

#include <ap_int.h>
#include <ap_utils.h>
#include <hls_stream.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  TestMem MemStreamHLS_mem;
  TestMem& memreq  = MemStreamHLS_mem;
  TestMem& memresp = MemStreamHLS_mem;
#else
  hls::stream<mem::MemReqMsg>  memreq;
  hls::stream<mem::MemRespMsg> memresp;
#endif

//------------------------------------------------------------------------
// test_basic
//------------------------------------------------------------------------

void test_basic()
{
  mem::InMemStream is(0x1000);
  int b;
  is >> b; // mem read

  mem::OutMemStream os(0x1004);
  os << b; // mem write

  int a = 0x0a0a0a0a;
  os << a; // mem write
}

//------------------------------------------------------------------------
// test_multiple
//------------------------------------------------------------------------

void test_multiple()
{
  mem::InMemStream is(0x1000);
  int a;
  int b;
  int c;
  is >> a >> b >> c; // 3x mem read

  mem::OutMemStream os(0x2000);
  os << c << b << a; // 3x mem write

  int d = 0x0b0b0b0b;
  int e = 0x0c0c0c0c;
  int f = 0x0d0d0d0d;
  os << d << e << f; // 3x mem write
}

//------------------------------------------------------------------------
// Example Struct
//------------------------------------------------------------------------

struct Foo {
  int   a;
  short b;
  char  c;
};

bool operator==( const Foo& lhs, const Foo& rhs )
{
  return (lhs.a == rhs.a) && (lhs.b == rhs.b) && (lhs.c == rhs.c);
}

std::ostream& operator<<( std::ostream& os, const Foo& foo )
{
  os << "{" << foo.a << "," << foo.b << "," << foo.c << "}";
  return os;
}

namespace mem {

  OutMemStream& operator<<( OutMemStream& os, const Foo& foo )
  {
    os << foo.a << foo.b << foo.c;
    os.addr += 1; // padding
    return os;
  }

  InMemStream& operator>>( InMemStream& is, Foo& foo )
  {
    is >> foo.a >> foo.b >> foo.c;
    is.addr += 1; // padding
    return is;
  }

}

//------------------------------------------------------------------------
// test_struct
//------------------------------------------------------------------------

void test_struct()
{
  mem::InMemStream is(0x1000);
  Foo foo1;
  is >> foo1; // 3x mem read

  mem::OutMemStream os(0x2000);
  os << foo1; // 3x mem write

  Foo foo2;
  foo2.a = 0x00000042;
  foo2.b = 0x0400;
  foo2.c = 'x'; // hex is 0x78
  os << foo2; // 3x mem write
}

//------------------------------------------------------------------------
// test_mixed
//------------------------------------------------------------------------

void test_mixed()
{
  mem::InMemStream is(0x1000);
  Foo foo1;
  Foo foo2;
  int a;
  is >> foo1 >> a >> foo2; // 7x mem read

  Foo foo3;
  foo3.a = foo1.a;
  foo3.b = foo2.b;
  foo3.c = foo2.c;

  Foo foo4;
  foo4.a = foo2.a;
  foo4.b = foo1.b;
  foo4.c = foo1.c;

  mem::OutMemStream os(0x2000);
  os << a << foo3 << foo4; // 7x mem write
}

//------------------------------------------------------------------------
// MemStreamHLS
//------------------------------------------------------------------------

using namespace xcel;

void MemStreamHLS
(
  hls::stream<XcelReqMsg>&  xcelreq,
  hls::stream<XcelRespMsg>& xcelresp
){
#pragma HLS INLINE

  XcelReqMsg req = xcelreq.read();
  int test_num = req.data;

  switch ( test_num ) {
    case 0: test_basic();    break;
    case 1: test_multiple(); break;
    case 2: test_struct();   break;
    case 3: test_mixed();    break;
  }

  xcelresp.write( XcelRespMsg( req.id, 0, req.type, req.opq ) );
}

