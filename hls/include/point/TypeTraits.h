//========================================================================
// PolyHS Type Traits Class
//========================================================================
// Author  : Shreesha Srinath, Ritchie Zhao
// Date    : April 29, 2015
// Project : Polymorphic Hardware Specialization
//
//
// Header file provides the traits interface for commonly used data-types

#ifndef POLYHS_TYPE_TRAITS_H
#define POLYHS_TYPE_TRAITS_H

#include "Details.h"
#include "Point.h"
#include "ReferenceProxy.h"

namespace polyhs {

  //----------------------------------------------------------------------
  // MetaData
  //----------------------------------------------------------------------
  // MetaData object stores the information we want to pass to polymorphic
  // hardware
  class MetaData {
    public:
      MetaData(){}
     ~MetaData(){}

      void init( unsigned int data )   { metadata = data;  }
      const unsigned int* getDataLoc() { return &metadata; }
      const unsigned int  getData()    { return metadata;  }
    private:
      unsigned int metadata;
  };

  //----------------------------------------------------------------------
  // MetaCreator
  //----------------------------------------------------------------------
  // Creates meta-data for primitive types
  template<typename T>
  class MetaCreator {
    public:
      MetaCreator() {

        // Create a data-type descriptor
        unsigned int data = 0;

        // Data-type descriptor
        //
        //  8 bits   8 bits 8 bits 8 bits
        // +--------+------+------+------------+
        // | offset | size | type | num_fields |
        // +--------+------+------+------------+

        SET_OFFSET( data, 0           );
        SET_SIZE  ( data, sizeof( T ) );
        SET_TYPE  ( data, 0           ); // Enum for types?
        SET_FIELDS( data, 0           );

        get()->init( data );
      }
      static MetaData* get() {
        static MetaData instance;
        return &instance;
      }
  };

  // Create an unique MetaData object for all primitive types
  // NOTE: NEED TO FIGURE OUT A WAY TO CREATE METADATA OBJECTS ONLY FOR THE
  // TYPES USED IN A PROGRAM
  MetaCreator<int>   intMetaObject;
  MetaCreator<char>  charMetaObject;
  MetaCreator<float> floatMetaObject;
  MetaCreator<short> shortMetaObject;

  //----------------------------------------------------------------------
  // TypeTraits
  //----------------------------------------------------------------------
  // Traits template for primitive data-types
  template<typename T>
  struct TypeTraits {

    // ReferenceProxy is an associated type
    typedef ReferenceProxy<T> reference;

    // MetaType is an associated type
    typedef MetaCreator<T> MetaType;

  };

  //----------------------------------------------------------------------
  // PointMetaData
  //----------------------------------------------------------------------
  // MetaData object stores the information we want to pass to polymorphic
  // hardware
  class PointMetaData {
    public:
      PointMetaData(){}
     ~PointMetaData(){}

      void init( unsigned int* data )
      {
        metadata[0] = data[0];
        metadata[1] = data[1];
        metadata[2] = data[2];
        metadata[3] = data[3];
      }

      const unsigned int* getDataLoc() { return &metadata[0]; }

    private:
      unsigned int metadata[4];
  };

  //----------------------------------------------------------------------
  // MetaCreator
  //----------------------------------------------------------------------
  // Creates meta-data for primitive types
  class PointMetaCreator {
    public:
      PointMetaCreator() {
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

      static PointMetaData* get() {
        static PointMetaData instance;
        return &instance;
      }
  };

  // Create an unique MetaData object for Point Type
  PointMetaCreator pointMetaData;

  //----------------------------------------------------------------------
  // TypeTraits -- Point Specialization
  //----------------------------------------------------------------------
  template<>
  struct TypeTraits<Point> {

    // ReferenceProxy is an associated type
    typedef PointReferenceProxy reference;

    // MetaType is an associated type
    typedef PointMetaCreator    MetaType;

  };

}

#endif /*POLYHS_TYPE_TRAITS_H*/

