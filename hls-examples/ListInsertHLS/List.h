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

#include "Common.h"
#include "../common/Proxy.h"
#include "ListProxy.h"

//------------------------------------------------------------------------
// PolyHS list container
//------------------------------------------------------------------------
template<typename T>
class list {
  public:
    typedef size_t                          size_type;
    typedef PointerProxy< NodeProxy<T> >    node_ptr;
  
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
      typedef node_ptr                          pointer;
      typedef typename choose<isconst, const value_type, value_type>::type
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

      reference operator*() const {return (*p).m_value;}

      pointer get_ptr() { return p; }
    };

    typedef _iterator<T,false> iterator;
    typedef _iterator<T,true>  const_iterator;

  private:
    node_ptr m_node;
  
    Address get_node_mem() {
      return (Address)malloc(sizeof(T)+2*sizeof(Address));
    }

    node_ptr get_node( const T& val = T() ) {
      node_ptr node( get_node_mem() );
      node->m_value = val;
      
      //printf("Created new node\n");
      //printf("  Base: %lx\n", (long unsigned)(node->get_addr()));
      //printf("  Data: %d\n", (int)(node->m_value));

      return node;
    }

    void put_node( node_ptr p ) {
      #ifdef CPP_COMPILE
        free( p.get_addr() );
      #endif
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
    // Operators
    //--------------------------------------------------------------------
    list& operator=( const list& x );

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
#include "List.inl"

#endif /*POLYHS_LIST_H*/
