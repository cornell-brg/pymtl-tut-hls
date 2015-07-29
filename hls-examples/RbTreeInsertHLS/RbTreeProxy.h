//==============================================================
// RbTree proxies header
//==============================================================
// This file contains proxy objects for use in RbTree

#ifndef RBTREE_PROXY_H
#define RBTREE_PROXY_H

#include "Common.h"
#include "../common/Proxy.h"

typedef bool _RbTreeColorType;
const _RbTreeColorType s_RbTreeRed = false;
const _RbTreeColorType s_RbTreeBlack = true;

template<typename T> class _NodePtrProxy;

//--------------------------------------------------------------
// template _NodeProxy
//--------------------------------------------------------------
template<typename T>
struct _NodeProxy {
  typedef ValueProxy<_RbTreeColorType>  ColorType;
  typedef ValueProxy<T>                 ValueType;
  typedef _NodePtrProxy<_NodeProxy>     NodePtrProxy;

  ValueType      m_value;
  ColorType      m_color;
  NodePtrProxy   m_parent;
  NodePtrProxy   m_left;
  NodePtrProxy   m_right;

  //------------------------------------------------------------
  // Constructors
  //------------------------------------------------------------
  _NodeProxy( Address base_ptr )
    : m_value(  base_ptr),
      m_color(  base_ptr+PTR_SIZE ),
      m_parent( base_ptr+PTR_SIZE+PTR_SIZE ),
      m_left (  base_ptr+3*PTR_SIZE ),
      m_right(  base_ptr+4*PTR_SIZE )
  {}
  _NodeProxy( const _NodeProxy& p )
    : m_value(  p.m_value ),
      m_color(  p.m_color ),
      m_parent( p.m_parent ),
      m_left (  p.m_left ),
      m_right(  p.m_right )
  {}

  //XXX: This should be outside the class, but we get an error...
  Address get_addr() const {
    return m_value.get_addr();
  }
};

template<typename T>
void set_addr( _NodeProxy<T>& p, Address addr ) {
  p.m_value.set_addr(  addr );
  p.m_color.set_addr(  addr+PTR_SIZE );
  p.m_parent.set_addr( addr+PTR_SIZE+PTR_SIZE );
  p.m_left.set_addr(   addr+3*PTR_SIZE );
  p.m_right.set_addr(  addr+4*PTR_SIZE );
}

//--------------------------------------------------------------
// template _NodePtrProxy
//--------------------------------------------------------------
template<typename NodeType>
class _NodePtrProxy {
    typedef PointerProxy< NodeType >    NodePointer;
  
    AddressPtr m_addr;

  public:
    //----------------------------------------------------------
    // Constructors
    //----------------------------------------------------------
    explicit _NodePtrProxy( Address base_ptr )
      : m_addr( (AddressPtr)base_ptr )
    {}
    _NodePtrProxy( const _NodePtrProxy& p ) 
      : m_addr( p.m_addr )
    {}

    //----------------------------------------------------------
    // * and -> operators
    //----------------------------------------------------------
    NodeType operator* () const {
      return NodeType( (Address)*this );
    }
    /*NodeType operator-> () const {
      return NodeType( *this );
    }*/

    //----------------------------------------------------------
    // Pointer conversion
    //----------------------------------------------------------
    operator Address() const {
      #ifdef CPP_COMPILE
        return (Address)*m_addr;
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return (Address)mem_resp.data;
      #endif
    }

    //----------------------------------------------------------
    // == and != operators
    //----------------------------------------------------------
    /*bool operator==( const Address rhs ) const {
      return Address(*this) == rhs;
    }
    bool operator!=( const Address rhs ) const {
      return Address(*this) != rhs;
    }*/

    //----------------------------------------------------------
    // rvalue and lvalue uses
    //----------------------------------------------------------
    operator NodePointer() const {
      #ifdef CPP_COMPILE
        return NodePointer( (Address)*m_addr );
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return NodePointer( (Address)mem_resp.data );
      #endif
    }
    _NodePtrProxy& operator=( const NodePointer& p ) {
      #ifdef CPP_COMPILE
        *(m_addr) = (Address)(p.get_addr());
      #else
        memreq.write( MemReqMsg( p.get_addr(), PTR_SIZE, m_addr, 0, MSG_WRITE ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
      #endif
      return *this;
    }
    _NodePtrProxy& operator=( const _NodePtrProxy& x ) {
      return operator=( static_cast<NodePointer>( x ) );
    }

    //----------------------------------------------------------
    // other
    //----------------------------------------------------------
    AddressPtr get_addr() const { return m_addr; }
    void set_addr( const Address addr ) { m_addr = (AddressPtr)addr; }
};

#endif
