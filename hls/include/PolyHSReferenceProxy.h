#ifndef POLYHS_REFERENCE_PROXY
#define POLYHS_REFERENCE_PROXY

#include "PolyHSValue.h"

extern DstuIfaceType g_dstu_iface;

//----------------------------------------------------------------------
// PolyHSReferenceProxy
//----------------------------------------------------------------------
class PolyHSReferenceProxy {
  private:
    // data structure id
    DstuIdType m_ds_id;

    // index to an data element
    DstuIterType m_iter;

    // metadata
    const ap_uint<8> m_type;
    const ap_uint<8> m_fields;

  public:

    // Constructors
    //PolyHSReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ), m_type ( 0 ) {}
    PolyHSReferenceProxy( DstuIdType ds_id, DstuIterType iter, 
                    const ap_uint<8> type, const ap_uint<8> fields )
      : m_ds_id( ds_id ), m_iter( iter ), m_type( type ), m_fields( fields ) {}

    PolyHSReferenceProxy( const PolyHSReferenceProxy & p )
      : m_ds_id( p.m_ds_id ), m_iter( p.m_iter ),
        m_type( p.m_type ), m_fields( p.m_fields ) {}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object

    operator PolyHSValue() const
    {
      //#pragma HLS PIPELINE
      PolyHSValue p (m_type);
      DstuRespMsg resp;
      
      // primitive type
      if (m_fields == 0) {
        resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, 0);
        p.data[0] = resp.data;
      }
      // struct
      else {
        for (int i = 0; i < MAX_FIELDS; ++i) {
        //#pragma HLS UNROLL
          if (i < m_fields) {
            resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, i+1);
            p.data[i] = resp.data;
          }
        }
      }

      return p;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- type::value
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    PolyHSReferenceProxy& operator=( const PolyHSValue p )
    {
      //#pragma HLS PIPELINE
      DstuRespMsg resp;
      DstuDataType data;
      
      // primitive type
      if (m_fields == 0) {
        resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, 0, p.data[0]);
      }
      // struct
      else {
        for (int i = 0; i < MAX_FIELDS; ++i) {
        //#pragma HLS UNROLL
          if (i < m_fields)
            resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, i+1, p.data[i]);
        }
      }

      // do nothing!
      return *this;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- PolyHSReferenceProxy argument
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    PolyHSReferenceProxy& operator=( const PolyHSReferenceProxy& x )
    {
      return operator=( static_cast<PolyHSValue>( x ) );
    }
};

#endif
