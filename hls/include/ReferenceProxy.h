#ifndef REFERENCEPROXY_H
#define REFERENCEPROXY_H

#include "DtuIface.h"
#include "Types.h"

extern volatile DtuIfaceType g_dtu_iface;

//-------------------------------------------------------------------
// Class ReferenceProxy
//-------------------------------------------------------------------
template<typename T, int field=0>
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
      DtuRespType resp = dtu_read (g_dtu_iface, m_ds_id, m_iter);
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
      DtuRespType resp = dtu_write (g_dtu_iface, m_ds_id, m_iter, d);
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
  
//----------------------------------------------------------------------
// Point Specialization
//----------------------------------------------------------------------
template<>
class ReferenceProxy <Point> {
  private:
    // data structure id
    DtuIdType m_ds_id;

    // index to an data element
    DtuIterType m_iter;

  public:

    // Proxy members
    ReferenceProxy<short, 1> label;
    ReferenceProxy<int,   2> x;
    ReferenceProxy<int,   3> y;

    // Constructors
    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ) {}
    ReferenceProxy( DtuIdType ds_id, DtuIterType iter )
      : m_ds_id( ds_id ), m_iter( iter ) {}

    ~ReferenceProxy(){}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object

    operator Point() const
    {
      Point p;
      DtuRespType resp;
      
      // read p.label
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter);
      p.label = DTU_RESP_DATA(resp);

      // read p.x, identical to p.label for now
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter);
      p.x = DTU_RESP_DATA(resp);
      
      // read p.x, identical to p.label for now
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter);
      p.y = DTU_RESP_DATA(resp);

      return p;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- type::value
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    ReferenceProxy& operator=( Point p )
    {
      DtuRespType resp;
      DtuDataType data;

      // set label
      data = p.label;
      resp = dtu_write (g_dtu_iface, m_ds_id, m_iter, data);
      
      // set x, identical to p.label for now
      data = p.x;
      resp = dtu_write (g_dtu_iface, m_ds_id, m_iter, data);

      // set y, identical to p.label for now
      data = p.y;
      resp = dtu_write (g_dtu_iface, m_ds_id, m_iter, data);

      // allows for "chaining" i.e. a = b = c = d;
      return *this;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- PointReferenceProxy argument
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    ReferenceProxy& operator=( const ReferenceProxy& x )
    {
      return operator=( static_cast<Point>( x ) );
    }
};

#endif
