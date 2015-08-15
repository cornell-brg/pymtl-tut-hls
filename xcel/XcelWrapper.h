//========================================================================
// XcelWrapper
//========================================================================
// The XcelWrapper class simply wraps a C++ function with an accelerator
// interface. The class is templated based on the number of accelerator
// registers the accelerator function requires. The configure() method
// configures the registers and the done function writes the result for the
// accelerator transaction

#include <hls_stream.h>

#include "XcelMsg.h"

extern hls::stream<xcel::XcelReqMsg>  xcelreq;
extern hls::stream<xcel::XcelRespMsg> xcelresp;

namespace xcel {

  template<int NumRegs>
  class XcelWrapper{
    public:

      // constructor
      INLINE XcelWrapper()
        : m_go( false )
      { }

      // configure
      INLINE void configure() {
        while ( !m_go ) {
          m_req = xcelreq.read();
          if ( m_req.type == XcelReqMsg::TYPE_READ ) {
            m_go  = true;
            break;
          }
          m_xregs[ m_req.addr ] = m_req.data;
          xcelresp.write( XcelRespMsg( m_req.id, 0, m_req.type, m_req.opq ) );
        }
      }

      // done
      INLINE void done( ap_uint<32> value ) {
        xcelresp.write( XcelRespMsg( m_req.id, value, m_req.type, m_req.opq ) );
        m_go = false;
      }

      // returns the request xcel register
      INLINE ap_uint<32> get_xreg( int addr ) {
        return m_xregs[ addr ];
      }

    private:
      ap_uint<32>  m_xregs[ NumRegs ];
      bool         m_go;
      XcelReqMsg   m_req;
  };

}


