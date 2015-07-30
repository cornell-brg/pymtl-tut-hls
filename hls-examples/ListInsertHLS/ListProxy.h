//========================================================================
// list proxies header
//========================================================================
// This file contains proxy objects for use with List

#ifndef LIST_PROXY_H
#define LIST_PROXY_H

#include <stdlib.h>
#include <stdio.h>
#include "Common.h"
#include "../common/Proxy.h"

// forward declarations
template<typename T> class NodePtrProxy;

//------------------------------------------------------------------------
// List NodeProxy
//------------------------------------------------------------------------
template<typename T>
struct NodeProxy {
  ValueProxy<T> m_value;
  NodePtrProxy<T> m_prev;
  NodePtrProxy<T> m_next;

  //----------------------------------------------------------------
  // Constructors
  //----------------------------------------------------------------
  NodeProxy( Address base_ptr )
    : m_value( base_ptr ),
      m_prev( base_ptr+m_value.size() ),
      m_next( base_ptr+m_value.size()+PTR_SIZE )
  {}
  NodeProxy( const NodeProxy& p ) 
    : m_value( p.m_value ),
      m_prev( p.m_prev ),
      m_next( p.m_next )
  {}


  //XXX: This should be outside the class, but we get an error
  Address get_addr() const {
    return m_value.get_addr();
  }
};

template<typename T>
void set_addr( NodeProxy<T>& p, Address addr ) {
  p.m_value.set_addr( addr );
  p.m_prev.set_addr( addr+p.m_value.size() );
  p.m_next.set_addr( addr+p.m_value.size()+PTR_SIZE );
}

//------------------------------------------------------------------------
// List NodePtrProxy
//------------------------------------------------------------------------
template<typename T>
class NodePtrProxy {
    typedef PointerProxy< NodeProxy<T> > NodePointer;

    // base ptr of the NodeProxy pointed to by this Ptr
    AddressPtr m_addr;

  public:
    //----------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------
    NodePtrProxy( Address base_ptr )
      : m_addr( (AddressPtr)base_ptr )
    {}
    NodePtrProxy( const NodePtrProxy& p )
      : m_addr( p.m_addr )
    {}

    //----------------------------------------------------------------
    // * and -> operators
    //----------------------------------------------------------------
    NodeProxy<T> operator* () const {
      return *(operator NodePointer());
    }
    NodePointer operator-> () {
      return operator NodePointer();
    }
    const NodePointer operator-> () const {
      return operator NodePointer();
    }

    //----------------------------------------------------------------
    // rvalue and lvalue uses
    //----------------------------------------------------------------
    operator NodePointer() const {
      #ifdef CPP_COMPILE
        return NodePointer( *m_addr );
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return NodePointer( mem_resp.data );
      #endif
    }
    NodePtrProxy& operator=( const NodePointer& p ) {
      #ifdef CPP_COMPILE
        *(m_addr) = (Address)p;
      #else
        memreq.write( MemReqMsg( (Address)p, PTR_SIZE, m_addr, 0, MSG_WRITE ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
      #endif
      return *this;
    }
    NodePtrProxy& operator=( const NodePtrProxy& x ) {
      return operator=( static_cast<NodePointer>( x ) );
    }

    //----------------------------------------------------------------
    // other
    //----------------------------------------------------------------
    void set_addr( const Address addr ) { m_addr = (AddressPtr)addr; }
};

#endif
