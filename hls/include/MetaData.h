#ifndef METADATA_H
#define METADATA_H

#include "Types.h"

//----------------------------------------------------------------------
// Macros to manipulate the fields of the data-type descriptor
//----------------------------------------------------------------------
// The data-type descriptor currently has the following format
//
//  8 bits   8 bits 8 bits 8 bits
// +--------+------+------+------------+
// | offset | size | type | num_fields |
// +--------+------+------+------------+
//

#define OFFSET_MASK  0xff
#define SIZE_MASK    0xff
#define TYPE_MASK    0xff
#define FIELDS_MASK  0xff

#define OFFSET_SHIFT 0x18
#define SIZE_SHIFT   0x10
#define TYPE_SHIFT   0x8
#define FIELDS_SHIFT 0x0

#define SET_OFFSET( x, off  )  (x) = (((x) & ~(OFFSET_MASK<<OFFSET_SHIFT)) | ((off)<<OFFSET_SHIFT))
#define SET_SIZE( x, size )    (x) = (((x) & ~(SIZE_MASK<<SIZE_SHIFT))     | ((size)<<SIZE_SHIFT))
#define SET_TYPE( x, type )    (x) = (((x) & ~(TYPE_MASK<<TYPE_SHIFT))     | ((type)<<TYPE_SHIFT))
#define SET_FIELDS( x, field ) (x) = (((x) & ~(FIELDS_MASK<<FIELDS_SHIFT)) | ((field)<<FIELDS_SHIFT))

#define GET_OFFSET( x ) ((x) & ~(OFFSET_MASK<<OFFSET_SHIFT))
#define GET_SIZE( x ) ((x) & ~(SIZE_MASK<<SIZE_SHIFT))
#define GET_TYPE( x ) ((x) & ~(TYPE_MASK<<TYPE_SHIFT))
#define GET_FIELDS( x ) ((x) & ~(FIELDS_MASK<<FIELDS_SHIFT))

//----------------------------------------------------------------------
// MetaData
//----------------------------------------------------------------------
// MetaData object stores the information we want to pass to polymorphic
// hardware
#define N_FIELDS 11
class MetaData {
  private:
    unsigned int metadata[N_FIELDS];
  public:
    MetaData() {
      for (int i = 0; i < N_FIELDS; ++i)
        metadata[i] = 0;
    }
   ~MetaData() { }

    void init( unsigned int data[] ) {
      for (int i = 0; i < N_FIELDS; ++i)
        metadata[i] = data[i];
    }
    const unsigned int* getDataLoc() { return metadata; }
    const unsigned int  getData()    { return metadata[0];  }
};

//----------------------------------------------------------------------
// MetaCreator
// Creates meta-data for primitive types
//----------------------------------------------------------------------
template<typename T>
class MetaCreator {
  public:
    MetaCreator() {

      // Create a data-type descriptor
      unsigned int data[N_FIELDS];
      data[0] = 0;

      // Data-type descriptor
      //
      //  8 bits   8 bits 8 bits 8 bits
      // +--------+------+------+------------+
      // | offset | size | type | num_fields |
      // +--------+------+------+------------+

      SET_OFFSET( data[0], 0           );
      SET_SIZE  ( data[0], sizeof( T ) );
      SET_TYPE  ( data[0], 0           ); // Enum for types?
      SET_FIELDS( data[0], 0           );

      get()->init( data );
    }

    static MetaData* get() {
      static MetaData instance;
      return &instance;
    }
};

//----------------------------------------------------------------------
// MetaCreator - Point Specialization
//----------------------------------------------------------------------
template<>
class MetaCreator <Point> {
  public:
    MetaCreator() {
      unsigned int data[4];

      // descripter for point
      SET_OFFSET( data[0], 0               );
      SET_SIZE  ( data[0], sizeof( Point ) );
      SET_TYPE  ( data[0], 1               );
      SET_FIELDS( data[0], 3               );

      // descriptor for label
      SET_OFFSET( data[1], 0               );
      SET_SIZE  ( data[1], sizeof( short ) );
      SET_TYPE  ( data[1], 0               );
      SET_FIELDS( data[1], 0               );

      // descriptor for x
      SET_OFFSET( data[2], 4               );
      SET_SIZE  ( data[2], sizeof( int   ) );
      SET_TYPE  ( data[2], 0               );
      SET_FIELDS( data[2], 0               );

      // descriptor for y
      SET_OFFSET( data[3], 8               );
      SET_SIZE  ( data[3], sizeof( int   ) );
      SET_TYPE  ( data[3], 0               );
      SET_FIELDS( data[3], 0               );

      get()->init( data );
    }

    static MetaData* get() {
      static MetaData instance;
      return &instance;
    }
};
#endif
