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

// forward declarations
template<typename T> class _NodePtrProxy;

//--------------------------------------------------------------
// template _NodeProxy
//--------------------------------------------------------------
template<typename T>
struct _NodeProxy {
  typedef ValueProxy<T>                 ValueType;
  typedef ValueProxy<_RbTreeColorType>  ColorType;
  typedef _NodePtrProxy<_NodeProxy>     NodePtrProxy;
  typedef PointerProxy<_NodeProxy>      NodePointer;

  ValueType      m_value;
  ColorType      m_color;
  NodePtrProxy   m_parent;
  NodePtrProxy   m_left;
  NodePtrProxy   m_right;

  //------------------------------------------------------------
  // Constructors
  //------------------------------------------------------------
  _NodeProxy( Address base_ptr )
    : m_value( base_ptr ),
      m_color(  base_ptr+m_value.size()),
      m_parent( base_ptr+m_value.size()+m_color.size()),
      m_left (  base_ptr+m_value.size()+m_color.size()+PTR_SIZE ),
      m_right(  base_ptr+m_value.size()+m_color.size()+2*PTR_SIZE )
  {}
  _NodeProxy( const _NodeProxy& p )
    : m_value(  p.m_value ),
      m_color(  p.m_color ),
      m_parent( p.m_parent ),
      m_left (  p.m_left ),
      m_right(  p.m_right )
  {}

  size_t total_size() {
    return m_value.size()+m_color.size()+3*PTR_SIZE;
  }

  //------------------------------------------------------------
  // Methods
  //------------------------------------------------------------
  static NodePointer s_minimum( NodePointer x ) {
    while( x->m_left != 0 ) x = x->m_left;
    return x;
  }
  static NodePointer s_maximum( NodePointer x ) {
    while( x->m_right != 0 ) x = x->m_right;
    return x;
  }

  //XXX: This should be outside the class, but we get an error...
  Address get_addr() const {
    return m_value.get_addr();
  }
};

template<typename T>
void set_addr( _NodeProxy<T>& p, Address addr ) {
  p.m_value.set_addr(  addr );
  p.m_color.set_addr(  addr+p.m_value.size() );
  p.m_parent.set_addr( addr+p.m_value.size()+p.m_color.size());
  p.m_left.set_addr(   addr+p.m_value.size()+p.m_color.size()+PTR_SIZE );
  p.m_right.set_addr(  addr+p.m_value.size()+p.m_color.size()+2*PTR_SIZE );
}

//--------------------------------------------------------------
// template _NodePtrProxy
//--------------------------------------------------------------
template<typename NodeType>
class _NodePtrProxy {
    typedef PointerProxy<NodeType>    NodePointer;

    // base ptr of the NodeProxy pointed to by this Ptr
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
      return NodeType( *m_addr );
    }
    NodePointer operator-> () {
      return operator NodePointer();
    }
    const NodePointer operator-> () const {
      return operator NodePointer();
    }

    //----------------------------------------------------------
    // rvalue and lvalue uses
    //----------------------------------------------------------
    operator NodePointer() const {
      DB_PRINT(("NodePtrProxy: read at loc %llu -> %llu\n",
            (long long unsigned)m_addr, (long long unsigned)*m_addr));
      #ifdef CPP_COMPILE
        return NodePointer( *m_addr );
      #else
        memreq.write( MemReqMsg( 0, PTR_SIZE, m_addr, 0, MSG_READ ) );
        ap_wait();
        MemRespMsg mem_resp = memresp.read();
        return NodePointer( mem_resp.data );
      #endif
    }
    _NodePtrProxy& operator=( const NodePointer& p ) {
      DB_PRINT(("NodePtrProxy: write at loc %llu -> %llu\n",
            (long long unsigned)m_addr, (long long unsigned)*m_addr));
      #ifdef CPP_COMPILE
        *(m_addr) = (Address)p;
      #else
        memreq.write( MemReqMsg( (Address)p, PTR_SIZE, m_addr, 0, MSG_WRITE ) );
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
    Address to_address() const { return (Address)(operator NodePointer()); }
    Address get_addr() const { return (Address)m_addr; }
    void set_addr( const Address addr ) { m_addr = (AddressPtr)addr; }
};

template<typename NodeType>
inline bool operator==( const _NodePtrProxy<NodeType>& lhs,
                        const _NodePtrProxy<NodeType>& rhs ) {
  return (PointerProxy<NodeType>)lhs == (PointerProxy<NodeType>)rhs;
}
template<typename NodeType>
inline bool operator==( const _NodePtrProxy<NodeType>& lhs,
                        const Address rhs ) {
  return (PointerProxy<NodeType>)lhs == rhs;
}
template<typename NodeType>
inline bool operator!=( const _NodePtrProxy<NodeType>& lhs,
                        const Address rhs ) {
  return (PointerProxy<NodeType>)lhs != rhs;
}

template<typename NodeType>
inline bool operator==( const PointerProxy<NodeType>& lhs,
                        const _NodePtrProxy<NodeType>& rhs ) {
  return lhs == (PointerProxy<NodeType>)rhs;
}
template<typename NodeType>
inline bool operator!=( const PointerProxy<NodeType>& lhs,
                        const _NodePtrProxy<NodeType>& rhs ) {
  return lhs != (PointerProxy<NodeType>)rhs;
}

#endif
