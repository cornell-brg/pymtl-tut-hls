//========================================================================
// XcelWrapper
//========================================================================
// The XcelWrapper class simply wraps a C++ function with an accelerator
// interface. The class is templated based on the number of accelerator
// registers the accelerator function requires. The configure() method
// configures the registers and the done function writes the result for the
// accelerator transaction

#include <hls_stream.h>

#include "xcel/XcelMsg.h"

namespace xcel {

  template < int NumRegs >
  class XcelWrapper{
    public:

      // constructor

      XcelWrapper( hls::stream<XcelReqMsg>&  xcelreq,
                   hls::stream<XcelRespMsg>& xcelresp )
        : m_go( false ), m_xcelreq( xcelreq ), m_xcelresp( xcelresp )
      { }

      // configure
      void configure() {
        while ( !m_go ) {
          m_req = m_xcelreq.read();
          if ( m_req.type() == XcelReqMsg::TYPE_READ ) {
            m_go  = true;
            break;
          }
          m_xregs[ m_req.addr() ] = m_req.data();
          m_xcelresp.write( XcelRespMsg( m_req.opq(), 1, 0, m_req.id() ) );
        }
      }

      // done
      void done( ap_uint<32> result ) {
        m_xcelresp.write( XcelRespMsg( m_req.opq(), m_req.type(), result, m_req.id() ) );
        m_go = false;
      }

      // returns the request xcel register
      ap_uint<32> get_xreg( int addr ) {
        return m_xregs[ addr ];
      }

    private:
      ap_uint<32>  m_xregs[ NumRegs ];
      bool         m_go;
      XcelReqMsg   m_req;

      hls::stream<XcelReqMsg>&  m_xcelreq;
      hls::stream<XcelRespMsg>& m_xcelresp;
  };

}


