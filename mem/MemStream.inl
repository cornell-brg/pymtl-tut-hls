//========================================================================
// MemStream
//========================================================================

#include "MemMsg.h"
#include <ap_utils.h>

#ifdef XILINX_VIVADO_HLS_TESTING
  #include "TestMem.h"
  extern TestMem& memreq;
  extern TestMem& memresp;
#else
  extern hls::stream<MemReqMsg>  memreq;
  extern hls::stream<MemRespMsg> memresp;
#endif

namespace mem {

  //----------------------------------------------------------------------
  // OutMemStream::OutMemStream()
  //----------------------------------------------------------------------

  inline
  OutMemStream::OutMemStream( unsigned int addr_ )
    : addr(addr_)
  {
    #ifdef XILINX_VIVADO_HLS_TESTING
      assert( addr != 0 );
    #endif
  }

  //----------------------------------------------------------------------
  // InMemStream::InMemStream()
  //----------------------------------------------------------------------

  inline
  InMemStream::InMemStream( unsigned int addr_ )
    : addr(addr_)
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
    memreq.write( MemReqMsg( value, 1, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, bool& value )
  {
    memreq.write( MemReqMsg( 0, 1, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<bool>(resp.data);
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned char
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned char value )
  {
    memreq.write( MemReqMsg( value, 1, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned char& value )
  {
    memreq.write( MemReqMsg( 0, 1, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<unsigned char>(resp.data);
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // char
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, char value )
  {
    memreq.write( MemReqMsg( value, 1, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 1;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, char& value )
  {
    memreq.write( MemReqMsg( 0, 1, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<char>(resp.data);
    is.addr += 1;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned short
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned short value )
  {
    memreq.write( MemReqMsg( value, 2, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 2;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned short& value )
  {
    memreq.write( MemReqMsg( 0, 2, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<unsigned short>(resp.data);
    is.addr += 2;
    return is;
  }

  //----------------------------------------------------------------------
  // short
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, short value )
  {
    memreq.write( MemReqMsg( value, 2, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 2;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, short& value )
  {
    memreq.write( MemReqMsg( 0, 2, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<short>(resp.data);
    is.addr += 2;
    return is;
  }

  //----------------------------------------------------------------------
  // unsigned int
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, unsigned int value )
  {
    memreq.write( MemReqMsg( value, 4, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 4;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, unsigned int& value )
  {
    memreq.write( MemReqMsg( 0, 4, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<unsigned int>(resp.data);
    is.addr += 4;
    return is;
  }

  //----------------------------------------------------------------------
  // int
  //----------------------------------------------------------------------

  inline
  OutMemStream& operator<<( OutMemStream& os, int value )
  {
    memreq.write( MemReqMsg( value, 4, os.addr, 0, MemReqMsg::TYPE_WRITE ) );
    ap_wait();
    memresp.read();
    os.addr += 4;
    return os;
  }

  inline
  InMemStream& operator>>( InMemStream& is, int& value )
  {
    memreq.write( MemReqMsg( 0, 4, is.addr, 0, MemReqMsg::TYPE_READ ) );
    ap_wait();
    MemRespMsg resp = memresp.read();
    value = static_cast<int>(resp.data);
    is.addr += 4;
    return is;
  }

}

