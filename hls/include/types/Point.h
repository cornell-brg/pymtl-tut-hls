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
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter, 1);
      p.label = DTU_RESP_DATA(resp);

      // read p.x
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter, 2);
      p.x = DTU_RESP_DATA(resp);
      
      // read p.x
      resp = dtu_read (g_dtu_iface, m_ds_id, m_iter, 3);
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
      resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, 1, data);
      
      // set x
      data = p.x;
      resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, 2, data);

      // set y
      data = p.y;
      resp = dtu_write_field (g_dtu_iface, m_ds_id, m_iter, 3, data);

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
