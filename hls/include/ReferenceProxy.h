#ifndef REFERENCEPROXY_H
#define REFERENCEPROXY_H

#include "DtuIface.h"

extern volatile DtuIfaceType g_dtu_iface;

//-------------------------------------------------------------------
// Class ReferenceProxy
//-------------------------------------------------------------------
template<typename T, unsigned field=0>
class ReferenceProxy {

  private:
    // data structure id
    DtuIdType m_ds_id;

    // index to an data element
    DtuIterType m_iter;

  public:

    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ) {}
    ReferenceProxy( DtuIdType ds_id, DtuIterType iter )
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
      DtuRespType resp = dtu_read (g_dtu_iface, m_ds_id, m_iter, field);
      T data = DTU_RESP_DATA(resp);
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
      DtuDataType d = data;
      DtuRespType resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, field, d);
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
