//========================================================================
// ArrayMemPortAdapter
//========================================================================
// This class provides a standard array interface, but the implementation
// essentially turns reads/writes into memory requests sent over an HLS
// stream interface. This is analogous to the pclib.fl.ListMemPortAdapter
// in PyMTL.
//
// XXX: NOTE: The class currently works only for 32-bit data values. NEED
// TO ADD SUPPORT FOR OTHER WIDTHS.

#ifndef MEM_ARRAY_MEMPORT_ADAPTER
#define MEM_ARRAY_MEMPORT_ADAPTER

#include "MemMsg.h"

namespace mem {

  template<typename StreamReq, typename StreamResp>
  class ArrayMemPortAdapter {

    public:

      class ArrayMemPortAdapterProxy {

        public:

          INLINE ArrayMemPortAdapterProxy( StreamReq&  memreq,
                                           StreamResp& memresp,
                                           int addr )
            : m_memreq(memreq), m_memresp(memresp), m_addr(addr)
          { }

          // cast
          INLINE operator int() const
          {
            // memory read request

            m_memreq.write( MemReqMsg( 0, 0, m_addr, 0, MemReqMsg::TYPE_READ ) );
            ap_wait();

            // memory read response

            MemRespMsg resp = m_memresp.read();

            // Return the data

            return static_cast<int>(resp.data);
          }

          // lvalue use of the proxy object
          INLINE ArrayMemPortAdapterProxy& operator=( int value )
          {
            // memory write request

            m_memreq.write( MemReqMsg( value, 0, m_addr, 0, MemReqMsg::TYPE_WRITE ) );
            ap_wait();

            // memory write response

            MemRespMsg resp = m_memresp.read();

            // allows for "chaining" i.e. a = b = c = d;

            return *this;
          }

        private:

          StreamReq&  m_memreq;
          StreamResp& m_memresp;
          int         m_addr;

      };

      INLINE ArrayMemPortAdapter( StreamReq&  memreq,
                                  StreamResp& memresp,
                                  int base, int size )
        : m_memreq(memreq), m_memresp(memresp), m_base(base), m_size(size)
      { }

      // Size
      INLINE int size() const
      {
        return m_size;
      }

      // Write
      INLINE ArrayMemPortAdapterProxy operator[]( ap_uint<32> i )
      {
        return ArrayMemPortAdapterProxy( m_memreq, m_memresp, (m_base + i*4) );
      }

      // Read
      INLINE ap_int<32> operator[]( ap_uint<32> i ) const
      {
        // memory read request

        m_memreq.write( MemReqMsg( 0, 0, (m_base + i*4), 0, MemReqMsg::TYPE_READ ) );
        ap_wait();

        // memory read response

        MemRespMsg resp = m_memresp.read();

        // Return the data

        return resp.data;
      }

    private:

      StreamReq&  m_memreq;
      StreamResp& m_memresp;
      int         m_base;
      int         m_size;
  };

}

#endif
