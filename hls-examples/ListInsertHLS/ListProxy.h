//========================================================================
// PolyHS list header
//========================================================================
// Authors : Shreesha Srinath, Ritchie Zhao
// Date    : September 17, 2014
// Project : Polymorphic Hardware Specialization
//
// Linked list container implementation inspired from C++ STL.

#ifndef POLYHS_LIST_H
#define POLYHS_LIST_H

#include <cstddef>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// forward declarations
template<typename T> class ValueProxy;
template<typename T> class NodePtrProxy;
template<typename T> struct NodeProxy;
template<typename T> class NodeProxyPointer;

//------------------------------------------------------------------------
// List NodePtrProxy
//------------------------------------------------------------------------
template<typename T>
class NodePtrProxy {
  public:
    // base ptr of the NodeProxy pointed to by this Ptr
    char** m_addr;

    // construct a NodePtrProxy from a base pointer and offset
    NodePtrProxy( char* base_ptr )
      : m_addr( (char**)base_ptr )
    {}
    // copy constructor
    NodePtrProxy( const NodePtrProxy& p ) 
      : m_addr( p.m_addr )
    {}

    // dereference operator constructs the NodeProxy pointed to by this Ptr
    //TODO: mem read request
    NodeProxy<T> operator* () const {
      return NodeProxy<T>( *m_addr );
    }
    // arrow operator returns a pointer to the constructed NodeProxy
    /*NodePtrProxy<T> operator-> () const {
      return NodePtrProxy( *this );
    }*/

    // rvalue use
    //TODO: mem read request
    operator NodeProxyPointer<T> () const {
      return NodeProxyPointer<T>(*m_addr);
    }
    // lvalue use
    //TODO: mem write request
    NodePtrProxy& operator=( const NodeProxyPointer<T>& p ) {
      *(m_addr) = (char*)(p.get_ptr());
      return *this;
    }
    // lvalue copy
    NodePtrProxy& operator=( const NodePtrProxy& x ) {
      return operator=( static_cast<NodeProxyPointer<T> >( x ) );
    }

    // debug
    char** get_ptr() const { return m_addr; }
};

//------------------------------------------------------------------------
// ValueProxy
//------------------------------------------------------------------------
template<typename T>
class ValueProxy {
  // pointer to the data managed by this proxy
  char* m_addr;

  public:
    ValueProxy( char* addr ) : m_addr( addr ) {}  
    ValueProxy( const ValueProxy& p ) : m_addr( p.m_addr ) {}  
    // rvalue use
    //TODO: Mem read request at address m_addr
    operator T() const {
      return *((T*)m_addr);
    }
    // lvalue use
    //TODO: Mem write request at address m_addr
    ValueProxy& operator= ( T data ) {
      *((T*)m_addr) = data;
      return *this;
    }
    // lvalue copy
    ValueProxy& operator=( const ValueProxy& x ) {
      return operator=( static_cast<T>( x ) );
    }

    char* get_ptr() const { return m_addr; }
};

//------------------------------------------------------------------------
// List NodeProxy
//------------------------------------------------------------------------
template<typename T>
struct NodeProxy {
  ValueProxy<T> m_value;
  NodePtrProxy<T> m_prev;
  NodePtrProxy<T> m_next;

  NodeProxy( char* base_ptr )
    : m_value( base_ptr ),
      m_prev( base_ptr+sizeof(T) ),
      m_next( base_ptr+sizeof(T)+sizeof(char*) )
  {}
  NodeProxy( const NodeProxy& p ) 
    : m_value( p.m_value ),
      m_prev( p.m_prev ),
      m_next( p.m_next )
  {}

  char* get_ptr() const { return m_value.get_ptr(); }
};

//------------------------------------------------------------------------
// List NodeProxyPointer
//------------------------------------------------------------------------
template<typename T>
class NodeProxyPointer {
  char* m_addr;

  public:
    NodeProxyPointer( char* base_ptr )
      : m_addr( base_ptr )
    {}
    NodeProxyPointer( const NodeProxyPointer& p )
      : m_addr( p.m_addr )
    {}

    NodeProxy<T> operator* () const {
      return NodeProxy<T>( m_addr );
    }
    NodeProxy<T>* operator-> () const {
      return &(operator*());
    }
    
    NodeProxyPointer& operator=( const NodeProxyPointer& p ) {
      m_addr = p.m_addr;
      return *this;
    }

    char* get_ptr() const { return m_addr; }
};

template<typename T>
inline bool operator==( const NodeProxyPointer<T>& lhs, const NodeProxyPointer<T>& rhs ) {
  return lhs.get_ptr() == rhs.get_ptr();
}

//------------------------------------------------------------------------
// PolyHS list container
//------------------------------------------------------------------------
template<typename T>
class list {
  public:
    typedef size_t            size_type;
    typedef NodeProxy<T>      list_node;
  
  public:
    // These template structs let us use a single class for iterator
    // and const_iterator. We can choose between them using a bool.
    // The structs let us change typedefs based on the bool value.
    // http://www.drdobbs.com/the-standard-librarian-defining-iterato/184401331?pgno=1
    template <bool flag, class IsTrue, class IsFalse>
    struct choose;

    template <class IsTrue, class IsFalse>
    struct choose<true, IsTrue, IsFalse> {
      typedef IsTrue type;
    };
    
    template <class IsTrue, class IsFalse>
    struct choose<false, IsTrue, IsFalse> {
      typedef IsFalse type;
    };

    // Iterator Class
    template<typename T2, bool isconst=false>
    class _iterator
    {
    friend class list<T2>;

    public:
      typedef std::bidirectional_iterator_tag   iterator_category;
      typedef ptrdiff_t                         difference_type;
      typedef ValueProxy<T2>                    value_type;
      typedef NodeProxyPointer<T2>               pointer;
      typedef typename choose<isconst, const T2&, value_type>::type
                                                reference;
    protected:
      pointer p;

    public:
      // constructors
      _iterator() : p(0) {}
      _iterator (const _iterator<T2, false>& it) : p(it.p) {}
      _iterator (pointer ptr) : p(ptr) {}

      _iterator& operator= (const _iterator& rhs) {p=rhs.p; return *this;}
      _iterator& operator++() {p=(*p).m_next; return *this;}
      _iterator  operator++(int) {_iterator tmp(*this); operator++(); return tmp;}
      _iterator& operator--() {p=(*p).m_prev; return *this;}
      _iterator  operator--(int) {_iterator tmp(*this); operator--(); return tmp;}

      bool operator==(const _iterator& rhs) const {return p==rhs.p;}
      bool operator!=(const _iterator& rhs) const {return !(*this==rhs);}
      //bool operator< (const _iterator& rhs) const {return p<rhs.p;}
      //bool operator<=(const _iterator& rhs) const {return p<=rhs.p;}
      //bool operator> (const _iterator& rhs) const {return !(*this<=rhs);}
      //bool operator>=(const _iterator& rhs) const {return !(*this<rhs);}

      reference operator*() const {return ((*p).m_value);}
    };

    typedef _iterator<T,false> iterator;
    typedef _iterator<T,true>  const_iterator;

  private:
    NodeProxyPointer<T> m_node;
  
    char* get_node_mem() {
      return (char*)malloc(sizeof(T)+2*sizeof(char*));
    }
    NodeProxyPointer<T> get_node( const T& val = T() ) {
      NodeProxyPointer<T> node( get_node_mem() );
      (*node).m_value = val;
      
      printf("Created new node\n");
      printf("  Base: %x\n", (*node).get_ptr());
      printf("  Data: %d\n", (int)((*node).m_value));

      return node;
    }
    void put_node( NodeProxyPointer<T> p ) {
      free( p.get_ptr() );
    }

  public:
    //--------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------
    explicit list();
    explicit list( size_type n, const T& value = T() );
    list( iterator first, iterator last );
    list( const list& x );
    
    //--------------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------------
    ~list();
    
    //--------------------------------------------------------------------
    // size methods
    //--------------------------------------------------------------------
    size_type size() const;
    bool empty() const;

    //--------------------------------------------------------------------
    // Iterator methods
    //--------------------------------------------------------------------
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    iterator begin();
    iterator end();

    //--------------------------------------------------------------------
    // Mutators
    //--------------------------------------------------------------------
    void push_front( const T& val );
    void pop_front();
    void push_back( const T& val );
    void pop_back();
    iterator insert( const_iterator pos, const T& val );
    iterator insert( const_iterator pos, size_type n, const T& val );
    iterator insert( const_iterator pos, const_iterator first, const_iterator last );
    iterator erase( const_iterator pos );
    iterator erase( const_iterator first, const_iterator last );
    void swap (list& x);
    void resize( size_type n, const T& val=T() );
    void clear();

    //--------------------------------------------------------------------
    // Algorithms
    //--------------------------------------------------------------------
    /*void splice( const_iterator pos, list& x);
    void splice( const_iterator pos, list& x, const_iterator i );
    void splice( const_iterator pos, list& x, const_iterator first, const_iterator last );
    void remove( const T& val );
    void sort();*/
};


// Include the implementation files which contains implementations for the
// template functions
#include "ListProxy.inl"
#include "ListProxy.alg.inl"

#endif /*POLYHS_LIST_H*/
