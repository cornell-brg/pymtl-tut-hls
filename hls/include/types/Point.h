#ifndef POINT_H
#define POINT_H

#include "../ReferenceProxy.h"
#include "../MetaData.h"
#include "../TypeEnum.h"

//----------------------------------------------------------------------
// Point type
//----------------------------------------------------------------------
struct Point {
  short label;
  int x;
  int y;

  Point() : label( 0 ), x( 0 ), y( 0 ) {}
  Point(int x_, int y_) : label( 0 ), x( x_ ), y( y_ ) {}
  Point(short label_, int x_, int y_) : label( label_ ), x( x_ ), y( y_ ) {}
  Point(const unsigned array[]) : label( array[0] ), x( array[1] ), y( array[2] ) {}
  Point(const Point& p) : label( p.label ), x( p.x ), y( p.y ) {}

  //--------------------------------------------------------------------
  // Boolean operator with Int
  //--------------------------------------------------------------------
  bool operator==(const int n) const {
    return x == n && y == n;
  }
  bool operator<=(const int n) const {
    return x <= n && y <= n;
  }
  bool operator< (const int n) const {
    return x < n && y < n;
  }
  bool operator> (const int n) const { return !(*this <= n); }
  bool operator>=(const int n) const { return !(*this <  n); }
  bool operator!=(const int n) const { return !(*this == n); }

  //--------------------------------------------------------------------
  // Boolean operator with Self
  //--------------------------------------------------------------------
  bool operator==(const Point& p) const {
    return x == p.x && y == p.y;
  }
  bool operator<=(const Point& p) const {
    return x <= p.x && y <= p.y;
  }
  bool operator< (const Point& p) const {
    return x < p.x && y < p.y;
  }
  bool operator> (const Point& rhs) const { return !(*this <= rhs); }
  bool operator>=(const Point& rhs) const { return !(*this <  rhs); }
  bool operator!=(const Point& rhs) const { return !(*this == rhs); }
};

template<>
struct TypeEnum<Point> {
  static TYPE_ENUM get() { return TYPE_POINT; }
};

//----------------------------------------------------------------------
// ReferenceProxy - Point Specialization
//----------------------------------------------------------------------
template<>
class ReferenceProxy <Point> {
  private:
    // data structure id
    DstuIdType m_ds_id;

    // index to an data element
    DstuIterType m_iter;

  public:

    // Proxy members
    ReferenceProxy<short, 1> label;
    ReferenceProxy<int,   2> x;
    ReferenceProxy<int,   3> y;

    // Constructors
    ReferenceProxy() : m_ds_id( 0 ), m_iter( 0 ) {}
    ReferenceProxy( DstuIdType ds_id, DstuIterType iter )
      : m_ds_id( ds_id ), m_iter( iter ) {}

    ~ReferenceProxy(){}

    //----------------------------------------------------------------
    // Overloaded typecast
    //----------------------------------------------------------------
    // rvalue use of the proxy object

    operator Point() const
    {
      Point p;
      DstuRespMsg resp;
      
      // read p.label
      resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, 1);
      p.label = resp.data;

      // read p.x
      resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, 2);
      p.x = resp.data;
      
      // read p.x
      resp = dstu_read (g_dstu_iface, m_ds_id, m_iter, 3);
      p.y = resp.data;

      return p;
    }

    //----------------------------------------------------------------
    // Overloaded assignment operator -- type::value
    //----------------------------------------------------------------
    // lvalue use of the proxy object

    ReferenceProxy& operator=( Point p )
    {
      DstuRespMsg resp;
      DstuDataType data;

      // set label
      data = p.label;
      resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, 1, data);
      
      // set x
      data = p.x;
      resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, 2, data);

      // set y
      data = p.y;
      resp = dstu_write (g_dstu_iface, m_ds_id, m_iter, 3, data);

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

//----------------------------------------------------------------------
// MetaCreator - Point Specialization
//----------------------------------------------------------------------
template<>
class MetaCreator <Point> {
  public:
    MetaCreator() {
      unsigned int data[MAX_FIELDS];

      // descripter for point
      SET_OFFSET( data[0], 0               );
      SET_SIZE  ( data[0], sizeof( Point ) );
      SET_TYPE  ( data[0], TYPE_POINT      );
      SET_FIELDS( data[0], 3               );

      // descriptor for label
      SET_OFFSET( data[1], 0               );
      SET_SIZE  ( data[1], sizeof( short ) );
      SET_TYPE  ( data[1], TYPE_SHORT      );
      SET_FIELDS( data[1], 0               );

      // descriptor for x
      SET_OFFSET( data[2], 4               );
      SET_SIZE  ( data[2], sizeof( int   ) );
      SET_TYPE  ( data[2], TYPE_INT        );
      SET_FIELDS( data[2], 0               );

      // descriptor for y
      SET_OFFSET( data[3], 8               );
      SET_SIZE  ( data[3], sizeof( int   ) );
      SET_TYPE  ( data[3], TYPE_INT        );
      SET_FIELDS( data[3], 0               );

      get()->init( data );
    }

    static MetaData* get() {
      static MetaData instance;
      return &instance;
    }
};

#endif
