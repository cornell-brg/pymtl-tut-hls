//========================================================================
// MemStream
//========================================================================

#include "mem/MemMsg.h"
#include <ap_utils.h>
#include <hls_stream.h>

namespace mem {

  //----------------------------------------------------------------------
  // OutMemStream::OutMemStream()
  //----------------------------------------------------------------------

  inline
  OutMemStream::OutMemStream( unsigned int addr_, MemReqStream& memreq,
                              MemRespStream& memresp )
    : addr(addr_), m_memreq( memreq ), m_memresp( memresp )
  {
    #ifdef XILINX_VIVADO_HLS_TESTING
      assert( addr != 0 );
    #endif
  }

  //----------------------------------------------------------------------
  // InMemStream::InMemStream()
  //----------------------------------------------------------------------

  inline
  InMemStream::InMemStream( unsigned int addr_, MemReqStream& memreq,
                            MemRespStream& memresp )
    : addr(addr_), m_memreq( memreq ), m_memresp( memresp )
  {
    #ifdef XILINX_VIVADO_HLS_TESTING
      assert( addr != 0 );
    #endif
  }

  //----------------------------------------------------------------------
  // bool
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, bool value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 1, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, bool& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 1, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<bool>(resp.data());
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned char
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned char value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 1, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned char& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 1, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<unsigned char>(resp.data());
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // char
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, char value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 1, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, char& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 1, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<char>(resp.data());
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned short
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned short value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 2, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 2;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned short& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 2, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<unsigned short>(resp.data());
    is.addr += 2;
    return is;
  }

  //----------------------------------------------------------------------
  // short
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, short value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 2, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 2;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, short& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 2, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<short>(resp.data());
    is.addr += 2;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned int
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned int value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 4, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 4;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned int& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 4, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<unsigned int>(resp.data());
    is.addr += 4;
    return is;
  }

  //----------------------------------------------------------------------
  // int
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, int value )
  {
    os.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_WRITE, 0, os.addr, 4, value ) );
    ap_wait();
    os.m_memresp.read();
    os.addr += 4;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, int& value )
  {
    is.m_memreq.write( MemReqMsg<>( MemReqMsg<>::TYPE_READ, 0, is.addr, 4, 0 ) );
    ap_wait();
    MemRespMsg<> resp = is.m_memresp.read();
    value = static_cast<int>(resp.data());
    is.addr += 4;
    return is;
  }

}

