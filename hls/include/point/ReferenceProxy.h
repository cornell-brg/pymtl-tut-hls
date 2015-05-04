//========================================================================
// PolyHS ReferenceProxy Class
//========================================================================
// Author  : Shreesha Srinath, Ritchie Zhao
// Date    : April 29, 2015
// Project : Polymorphic Hardware Specialization
//
//
// Header file for reference proxies

#ifndef POLYHS_REFERENCE_PROXY_H
#define POLYHS_REFERENCE_PROXY_H

#include "../ItuIface.h"
#include "Details.h"
#include "Point.h"

extern volatile ItuIfaceType g_itu_iface;

namespace polyhs {
  //----------------------------------------------------------------------
  // ReferenceProxy class
  //----------------------------------------------------------------------
  // Class provides a reference proxy object for primitive data-types
  template<typename T, int field=0>
  class ReferenceProxy {
    public:
      //------------------------------------------------------------------
      // Hardware iterator abstraction
      //------------------------------------------------------------------
      // Data members that serve as the state used by hardware models to
      // abstract a location in the sequence.
      //
      // NOTE: If the hardware iterator abstraction changes then we need
      // to modify these members and provide setters/getters if required.
      //
      //  8 bits  24 bits
      // +------+-----------+
      // | id   | iter_bits |
      // +------+-----------+

      // hardware iterator
      unsigned m_iter;

      // Constructors
      ReferenceProxy() : m_iter( 0 ) {}
      ReferenceProxy( unsigned iter ) : m_iter( iter ){}

      // Destructor
      ~ReferenceProxy(){}

      //------------------------------------------------------------------
      // Overloaded typecast
      //------------------------------------------------------------------
      // rvalue use of the proxy object

      operator T() const
      {
        //send a read request
        ItuRespType resp = itu_read (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter)
        );
        T data = resp & 0xFFFF;
        return data;
      }

      //------------------------------------------------------------------
      // Overloaded assignment operator -- type::value
      //------------------------------------------------------------------
      // lvalue use of the proxy object

      ReferenceProxy& operator=( T val )
      {
        int data = val & 0xFFFF;

        //send a write request
        int idata = data & 0xFFFF;
        ItuRespType resp = itu_write (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter),
            data
        );
        // verify first bit of resp is 1?
        return *this;
      }

      //------------------------------------------------------------------
      // Overloaded assignment operator -- ReferenceProxy argument
      //------------------------------------------------------------------
      // lvalue use of the proxy object

      ReferenceProxy& operator=( const ReferenceProxy& x )
      {
        return operator=( static_cast<T>( x ) );
      }
  };

  //----------------------------------------------------------------------
  // PointReferenceProxy
  //----------------------------------------------------------------------

  class PointReferenceProxy {
    private:
      //------------------------------------------------------------------
      // Hardware iterator abstraction
      //------------------------------------------------------------------
      // Data members that serve as the state used by hardware models to
      // abstract a location in the sequence.
      //
      // NOTE: If the hardware iterator abstraction changes then we need
      // to modify these members and provide setters/getters if required.
      //
      //  8 bits  24 bits
      // +------+-----------+
      // | id   | iter_bits |
      // +------+-----------+

      // hardware iterator
      unsigned m_iter;

    public:

      // Proxy members
      polyhs::ReferenceProxy<short,1> label;
      polyhs::ReferenceProxy<int,  2> x;
      polyhs::ReferenceProxy<int,  3> y;

      // Constructors
      PointReferenceProxy() : m_iter( 0 )
      { label.m_iter = x.m_iter = y.m_iter = 0; }

      PointReferenceProxy( unsigned iter )
      { m_iter = label.m_iter = x.m_iter = y.m_iter = iter; }

      // Destructor
      ~PointReferenceProxy(){}

      //----------------------------------------------------------------
      // Overloaded typecast
      //----------------------------------------------------------------
      // rvalue use of the proxy object

      operator Point() const
      {
        Point p;
        ItuRespType resp;
        
        // read p.label
        resp = itu_read (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter)
        );
        p.label = resp & 0xFFFF;

        // read p.x, identical to p.label for now
        resp = itu_read (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter)
        );
        p.x = resp & 0xFFFF;
        
        // read p.x, identical to p.label for now
        resp = itu_read (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter)
        );
        p.y = resp & 0xFFFF;

        return p;
      }

      //----------------------------------------------------------------
      // Overloaded assignment operator -- type::value
      //----------------------------------------------------------------
      // lvalue use of the proxy object

      PointReferenceProxy& operator=( Point p )
      {
        int data;
        ItuRespType resp;

        // set label
        data = p.label & 0xFFFF;
        resp = itu_write (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter), data
        );

        // set x, identical to p.label for now
        data = p.x & 0xFFFF;
        resp = itu_write (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter), data
        );

        // set y, identical to p.label for now
        data = p.y & 0xFFFF;
        resp = itu_write (
            g_itu_iface, GET_ID(m_iter), GET_ITER(m_iter), data
        );

        // allows for "chaining" i.e. a = b = c = d;
        return *this;
      }

      //----------------------------------------------------------------
      // Overloaded assignment operator -- PointReferenceProxy argument
      //----------------------------------------------------------------
      // lvalue use of the proxy object

      PointReferenceProxy& operator=( const PointReferenceProxy& x )
      {
        return operator=( static_cast<Point>( x ) );
      }
  };
}

#endif /*POLYHS_TYPE_TRAITS_H*/

