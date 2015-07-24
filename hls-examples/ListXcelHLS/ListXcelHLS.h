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

// forward declarations
template<typename T> class NodePtrProxy;
template<typename T> struct NodeProxy;

//------------------------------------------------------------------------
// List NodePtrProxy
//------------------------------------------------------------------------
template<typename T>
class NodePtrProxy {
  public:
    typedef NodeProxy<T>* pointer;
    typedef NodeProxy<T>& reference;

  private:
    pointer base_ptr;

  public:
    NodePtrProxy( pointer _bp )
      : base_ptr( _bp )
    { }

    // dereference operator constructs a NodeProxy with base address
    // equal to the value of base_ptr
    reference operator* () const {
      // load data
      /*unsigned data = *((unsigned*)Address);
      Address prev = *((Address*)(Address+4));
      Address next = *((Address*)(Address+8));*/
      return *base_ptr;
    }

    // arrow operator returns a pointer to the constructed NodeProxy
    pointer operator-> () const {
      return &(operator*());
    }

    // assignment
    NodePtrProxy& operator=( const NodePtrProxy& p ) {
      base_ptr = p.base_ptr;
      return *this;
    }
    NodePtrProxy& operator=( const pointer& p ) {
      base_ptr = p;
      return *this;
    }

    // conversion
    operator pointer() {
      return base_ptr;
    }
};

//------------------------------------------------------------------------
// List NodeProxy
//------------------------------------------------------------------------
template<typename T>
struct NodeProxy {
  T m_data;
  NodePtrProxy<T> m_prev;
  NodePtrProxy<T> m_next;

  NodeProxy( T data )
    : m_data( data ), m_prev( 0 ), m_next( 0 )
  {}
  NodeProxy( T data, NodePtrProxy<T> prev, NodePtrProxy<T> next )
    : m_data( data ), m_prev( prev ), m_next( next )
  {}

  NodePtrProxy<T> operator& () {
    return NodePtrProxy<T>(this);
  }
};

//------------------------------------------------------------------------
// PolyHS list container
//------------------------------------------------------------------------
template<typename T>
class list {
  public:
    typedef size_t            size_type;
    typedef NodeProxy<T>     list_node;
  
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
      typedef T2                                value_type;
      typedef list_node*                        pointer;
      typedef typename choose<isconst, const value_type&, value_type&>::type
                                                reference;
    protected:
      pointer p;

    public:
      // constructors
      _iterator() : p(0) {}
      _iterator (const _iterator<T2, false>& it) : p(it.p) {}
      _iterator (pointer ptr) : p(ptr) {}

      _iterator& operator= (const _iterator& rhs) {p=rhs.p; return *this;}
      _iterator& operator++() {p=p->m_next; return *this;}
      _iterator  operator++(int) {_iterator tmp(*this); operator++(); return tmp;}
      _iterator& operator--() {p=p->m_prev; return *this;}
      _iterator  operator--(int) {_iterator tmp(*this); operator--(); return tmp;}

      bool operator==(const _iterator& rhs) const {return p==rhs.p;}
      bool operator!=(const _iterator& rhs) const {return !(*this==rhs);}
      //bool operator< (const _iterator& rhs) const {return p<rhs.p;}
      //bool operator<=(const _iterator& rhs) const {return p<=rhs.p;}
      //bool operator> (const _iterator& rhs) const {return !(*this<=rhs);}
      //bool operator>=(const _iterator& rhs) const {return !(*this<rhs);}

      reference operator*() const {return p->m_data;}
    };

    typedef _iterator<T,false> iterator;
    typedef _iterator<T,true>  const_iterator;

  private:
    list_node* m_node;

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
    void splice( const_iterator pos, list& x);
    void splice( const_iterator pos, list& x, const_iterator i );
    void splice( const_iterator pos, list& x, const_iterator first, const_iterator last );
    void remove( const T& val );
    void sort();
};


// Include the implementation files which contains implementations for the
// template functions
#include "ListXcelHLS.inl"
#include "ListXcelHLS.alg.inl"

#endif /*POLYHS_LIST_H*/
