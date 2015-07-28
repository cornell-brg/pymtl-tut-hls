//========================================================================
// list proxies header
//========================================================================
// This file contains proxy objects for use with List

#ifndef LIST_PROXY_H
#define LIST_PROXY_H

#include <stdlib.h>
#include <stdio.h>
#include "Common.h"

// forward declarations
template<typename T> class ValueProxy;
template<typename T> class NodePtrProxy;
template<typename T> struct NodeProxy;
template<typename T> class NodeProxyPointer;

static unsigned PTR_SIZE = sizeof(void*);
//------------------------------------------------------------------------
// List NodePtrProxy
//------------------------------------------------------------------------
template<typename T>
class NodePtrProxy {
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
    // dereference operator constructs the NodeProxy pointed to by this Ptr
    NodeProxy<T> operator* () const {
      #ifdef CPP_COMPILE
        return NodeProxy<T>( (Address)*m_addr );
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return NodeProxy<T>( (Address) mem_resp.data );
      #endif
    }
    // arrow operator returns a pointer to the constructed NodeProxy
    /*NodePtrProxy<T> operator-> () const {
      return NodePtrProxy( *this );
    }*/

    //----------------------------------------------------------------
    // rvalue and lvalue uses
    //----------------------------------------------------------------
    operator NodeProxyPointer<T> () const {
      #ifdef CPP_COMPILE
        return NodeProxyPointer<T>( (Address)*m_addr );
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return NodeProxyPointer<T>( (Address)mem_resp.data );
      #endif
    }
    NodePtrProxy& operator=( const NodeProxyPointer<T>& p ) {
      #ifdef CPP_COMPILE
        *(m_addr) = (Address)(p.get_addr());
      #else
        memreq.write( MemReqMsg( p.get_addr(), PTR_SIZE, m_addr, 0, MSG_WRITE ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
      #endif
      return *this;
    }
    NodePtrProxy& operator=( const NodePtrProxy& x ) {
      return operator=( static_cast<NodeProxyPointer<T> >( x ) );
    }

    //----------------------------------------------------------------
    // other
    //----------------------------------------------------------------
    AddressPtr get_addr() const { return m_addr; }
    void set_addr( const Address addr ) { m_addr = (AddressPtr)addr; }
};

//------------------------------------------------------------------------
// ValueProxy
//------------------------------------------------------------------------
template<typename T>
class ValueProxy {
  // pointer to the data managed by this proxy
  Address m_addr;

  public:
    //----------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------
    ValueProxy( Address addr ) : m_addr( addr ) {}
    ValueProxy( const ValueProxy& p ) : m_addr( p.m_addr ) {}

    //----------------------------------------------------------------
    // rvalue and lvalue uses
    //----------------------------------------------------------------
    operator T() const {
      #ifdef CPP_COMPILE
        return *((T*)m_addr);
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return ((T)mem_resp.data);
      #endif
    }
    ValueProxy& operator= ( T data ) {
      #ifdef CPP_COMPILE
        *((T*)m_addr) = data;
      #else
        memreq.write( MemReqMsg( data, PTR_SIZE, m_addr, 0, MSG_WRITE ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
      #endif
      return *this;
    }
    ValueProxy& operator=( const ValueProxy& x ) {
      return operator=( static_cast<T>( x ) );
    }

    //----------------------------------------------------------------
    // other
    //----------------------------------------------------------------
    Address get_addr() const { return m_addr; }
    void set_addr( const Address addr ) { m_addr = addr; }
};

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
      m_prev( base_ptr+PTR_SIZE ),
      m_next( base_ptr+PTR_SIZE+PTR_SIZE )
  {}
  NodeProxy( const NodeProxy& p ) 
    : m_value( p.m_value ),
      m_prev( p.m_prev ),
      m_next( p.m_next )
  {}

  //----------------------------------------------------------------
  // other
  //----------------------------------------------------------------
  Address get_addr() const { return m_value.get_addr(); }
  void set_addr( const Address addr ) {
    m_value.set_addr( addr );
    m_prev.set_addr( addr+PTR_SIZE );
    m_next.set_addr( addr+PTR_SIZE+PTR_SIZE );
  }
};

//------------------------------------------------------------------------
// List NodeProxyPointer
//------------------------------------------------------------------------
template<typename T>
class NodeProxyPointer {
  Address m_addr;
  NodeProxy<T> m_obj_temp;

  public:
    //----------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------
    NodeProxyPointer( Address base_ptr )
      : m_addr( base_ptr ), m_obj_temp( base_ptr )
    {}
    NodeProxyPointer( const NodeProxyPointer& p )
      : m_addr( p.m_addr ), m_obj_temp( p.m_obj_temp.get_addr() )
    {}
    
    //----------------------------------------------------------------
    // * and -> operators
    //----------------------------------------------------------------
    NodeProxy<T> operator* () const {
      return NodeProxy<T>( m_addr );
    }
    NodeProxy<T>* operator-> () {
      m_obj_temp.set_addr( m_addr );
      return &m_obj_temp;
    }
    const NodeProxy<T>* operator-> () const {
      return &m_obj_temp;
    }
    
    //----------------------------------------------------------------
    // = operator
    //----------------------------------------------------------------
    NodeProxyPointer& operator=( const NodeProxyPointer& x ) {
      m_addr = x.m_addr;
      return *this;
    }

    //----------------------------------------------------------------
    // Other
    //----------------------------------------------------------------
    Address get_addr() const { return m_addr; }
};

template<typename T>
inline bool operator==( const NodeProxyPointer<T>& lhs,
                        const NodeProxyPointer<T>& rhs ) {
  return lhs.get_addr() == rhs.get_addr();
}

#endif
