#ifndef REFERENCEPROXY_H
#define REFERENCEPROXY_H

#include "interfaces.h"

extern DstuIfaceType g_dstu_iface;

//-------------------------------------------------------------------
// Class ReferenceProxy
//-------------------------------------------------------------------
template<typename T, unsigned field=0>
class ReferenceProxy {

  private:
    // data structure id
    DstuIdType m_ds_id;

    // index to an data element
    DstuIterType m_iter;

  public:

    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ) {}
    ReferenceProxy( DstuIdType ds_id, DstuIterType iter )
      : m_ds_id( ds_id ), m_iter( iter ) {}

    ~ReferenceProxy(){}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object
    
    operator T() const
    {
      //#pragma HLS INLINE self off
      //send a read request
      DstuRespMsg resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, field);
      T data = resp.data;
      return data;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- type::value
    //----------------------------------------------------------------
    // lvalue use of the proxy object
    
    ReferenceProxy& operator=( T data )
    {
      //#pragma HLS INLINE self off
      //send a write request
      DstuDataType d = data;
      DstuRespMsg resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, field, d);
      // verify first bit of resp is 1?
      return *this;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- PointReferenceProxy argument
    //----------------------------------------------------------------
    // lvalue use of the proxy object
    
    ReferenceProxy& operator=( const ReferenceProxy& x )
    {
      return operator=( static_cast<T>( x ) );
    }

}; // class ReferenceProxy
  
#endif
