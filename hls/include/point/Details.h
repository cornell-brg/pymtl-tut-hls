//========================================================================
// PolyHS Hardware Container Details
//========================================================================
// Author  : Shreesha Srinath
// Date    : April 22, 2015
// Project : Polymorphic Hardware Specialization
//
// This header provides ReferenceProxy objects for commonly used
// data-types and enums for container types

#ifndef POLYHS_HARDWARE_CONTAINER_DETAILS_H
#define POLYHS_HARDWARE_CONTAINER_DETAILS_H

namespace polyhs {

  //----------------------------------------------------------------------
  // ContainerType
  //----------------------------------------------------------------------

  struct ContainerType {
    enum {
      VECTOR,
      LIST
    };
  };

  //----------------------------------------------------------------------
  // Macros to manipulate the hardware iterator
  //----------------------------------------------------------------------
  // Helper macros that set the fields of the hardware iterator
  //
  //  8 bits  24 bits
  // +------+-----------+
  // | id   | iter_bits |
  // +------+-----------+
  //

  #define ID_MASK    0xff
  #define ID_SHIFT   0x18
  #define SET_ID( x, id ) x = ((x & ~(ID_MASK<<ID_SHIFT)) | (id<<ID_SHIFT))
  #define GET_ID( x ) ((x)>>ID_SHIFT & ID_MASK)

  #define ITER_MASK  0xffffff
  #define SET_ITER( x, iter ) x = ((x & ~(ITER_MASK)) | iter)
  #define GET_ITER( x ) ( x & ITER_MASK )

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

  #define SET_OFFSET( x, off  )  x = ((x & ~(OFFSET_MASK<<OFFSET_SHIFT)) | (off<<OFFSET_SHIFT))
  #define SET_SIZE( x, size )    x = ((x & ~(SIZE_MASK<<SIZE_SHIFT))     | (size<<SIZE_SHIFT))
  #define SET_TYPE( x, type )    x = ((x & ~(TYPE_MASK<<TYPE_SHIFT))     | (type<<TYPE_SHIFT))
  #define SET_FIELDS( x, field ) x = ((x & ~(FIELDS_MASK<<FIELDS_SHIFT)) | (field<<FIELDS_SHIFT))
}

#endif /*POLYHS_HARDWARE_CONTAINER_DETAILS_H*/
